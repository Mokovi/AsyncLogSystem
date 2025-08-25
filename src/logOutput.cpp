/**
 * @file logOutput.cpp
 * @brief 日志输出类实现
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 实现文件输出、控制台输出、网络输出等具体功能
 * @see log_output.hpp
 * @since 1.0.0
 */

#include "logOutput.hpp"
#include "logTypes.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <algorithm>

namespace async_log {

// FileOutput 实现
FileOutput::FileOutput(const std::string& path, size_t maxSize, int maxCount)
    : filePath_(path), currentFileSize_(0), maxFileSize_(maxSize), 
      maxFileCount_(maxCount), isOpen_(false) {
    openFile();
}

FileOutput::~FileOutput() {
    close();
}

FileOutput::FileOutput(FileOutput&& other) noexcept
    : filePath_(std::move(other.filePath_)), 
      fileStream_(std::move(other.fileStream_)),
      currentFileSize_(other.currentFileSize_),
      maxFileSize_(other.maxFileSize_),
      maxFileCount_(other.maxFileCount_),
      isOpen_(other.isOpen_) {
    other.isOpen_ = false;
    other.currentFileSize_ = 0;
}

FileOutput& FileOutput::operator=(FileOutput&& other) noexcept {
    if (this != &other) {
        close();
        filePath_ = std::move(other.filePath_);
        fileStream_ = std::move(other.fileStream_);
        currentFileSize_ = other.currentFileSize_;
        maxFileSize_ = other.maxFileSize_;
        maxFileCount_ = other.maxFileCount_;
        isOpen_ = other.isOpen_;
        
        other.isOpen_ = false;
        other.currentFileSize_ = 0;
    }
    return *this;
}

void FileOutput::write(const LogMessage& msg) {
    std::lock_guard<std::mutex> lock(fileMutex_);
    
    if (!isOpen_ && !openFile()) {
        return;
    }
    
    std::string formattedMsg = formatMessage(msg);
    fileStream_ << formattedMsg << std::endl;
    currentFileSize_ += formattedMsg.length() + 1; // +1 for newline
    
    // 检查是否需要轮转文件
    if (currentFileSize_ >= maxFileSize_) {
        rotateFile();
    }
}

void FileOutput::flush() {
    std::lock_guard<std::mutex> lock(fileMutex_);
    if (isOpen_) {
        fileStream_.flush();
    }
}

void FileOutput::close() {
    std::lock_guard<std::mutex> lock(fileMutex_);
    if (isOpen_) {
        fileStream_.close();
        isOpen_ = false;
    }
}

bool FileOutput::isAvailable() const {
    std::lock_guard<std::mutex> lock(fileMutex_);
    return isOpen_;
}

void FileOutput::setFilePath(const std::string& path) {
    std::lock_guard<std::mutex> lock(fileMutex_);
    close();
    filePath_ = path;
    openFile();
}

std::string FileOutput::getFilePath() const {
    std::lock_guard<std::mutex> lock(fileMutex_);
    return filePath_;
}

bool FileOutput::openFile() {
    try {
        // 确保目录存在
        std::filesystem::path path(filePath_);
        std::filesystem::create_directories(path.parent_path());
        
        fileStream_.open(filePath_, std::ios::app);
        if (fileStream_.is_open()) {
            isOpen_ = true;
            currentFileSize_ = fileStream_.tellp();
            return true;
        }
    } catch (const std::exception&) {
        // 忽略异常，返回false
    }
    return false;
}

void FileOutput::rotateFile() {
    close();
    
    try {
        std::filesystem::path path(filePath_);
        std::string baseName = path.stem().string();
        std::string extension = path.extension().string();
        
        // 删除最旧的文件
        for (int i = maxFileCount_ - 1; i >= 1; --i) {
            std::string oldName = baseName + "." + std::to_string(i) + extension;
            std::string oldPath = (path.parent_path() / oldName).string();
            
            if (std::filesystem::exists(oldPath)) {
                if (i == maxFileCount_ - 1) {
                    std::filesystem::remove(oldPath);
                } else {
                    std::string newName = baseName + "." + std::to_string(i + 1) + extension;
                    std::string newPath = (path.parent_path() / newName).string();
                    std::filesystem::rename(oldPath, newPath);
                }
            }
        }
        
        // 重命名当前文件
        std::string newName = baseName + ".1" + extension;
        std::string newPath = (path.parent_path() / newName).string();
        std::filesystem::rename(filePath_, newPath);
        
        // 重新打开文件
        openFile();
        currentFileSize_ = 0;
    } catch (const std::exception&) {
        // 忽略异常，尝试重新打开原文件
        openFile();
    }
}

std::string FileOutput::formatMessage(const LogMessage& msg) {
    std::ostringstream oss;
    oss << "[" << levelToString(msg.level) << "] "
        << std::chrono::duration_cast<std::chrono::seconds>(
               msg.timestamp.time_since_epoch()).count()
        << " " << msg.file << ":" << msg.line;
    
    if (!msg.function.empty()) {
        oss << " " << msg.function;
    }
    
    oss << " - " << msg.message;
    return oss.str();
}

// ConsoleOutput 实现
ConsoleOutput::ConsoleOutput(bool enableColor)
    : enableColor_(enableColor) {
}

void ConsoleOutput::write(const LogMessage& msg) {
    std::lock_guard<std::mutex> lock(consoleMutex_);
    
    std::string formattedMsg = formatMessage(msg);
    
    if (enableColor_) {
        std::cout << getColorCode(msg.level) << formattedMsg << getResetCode() << std::endl;
    } else {
        std::cout << formattedMsg << std::endl;
    }
}

void ConsoleOutput::flush() {
    std::lock_guard<std::mutex> lock(consoleMutex_);
    std::cout.flush();
}

void ConsoleOutput::close() {
    // 控制台输出不需要关闭
}

bool ConsoleOutput::isAvailable() const {
    return true; // 控制台总是可用的
}

void ConsoleOutput::setColorEnabled(bool enable) {
    enableColor_ = enable;
}

std::string ConsoleOutput::getColorCode(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "\033[36m"; // 青色
        case LogLevel::INFO:  return "\033[32m"; // 绿色
        case LogLevel::WARN:  return "\033[33m"; // 黄色
        case LogLevel::ERROR: return "\033[31m"; // 红色
        case LogLevel::FATAL: return "\033[35m"; // 紫色
        default:              return "\033[0m";  // 默认
    }
}

std::string ConsoleOutput::getResetCode() {
    return "\033[0m";
}

std::string ConsoleOutput::formatMessage(const LogMessage& msg) {
    std::ostringstream oss;
    oss << "[" << levelToString(msg.level) << "] "
        << std::chrono::duration_cast<std::chrono::seconds>(
               msg.timestamp.time_since_epoch()).count()
        << " " << msg.file << ":" << msg.line;
    
    if (!msg.function.empty()) {
        oss << " " << msg.function;
    }
    
    oss << " - " << msg.message;
    return oss.str();
}

// NetworkOutput 实现
NetworkOutput::NetworkOutput(const std::string& host, int port)
    : host_(host), port_(port), isConnected_(false) {
    // 这里应该实现实际的网络连接逻辑
    // 暂时只是模拟
}

void NetworkOutput::write(const LogMessage& msg) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    if (!isConnected_) {
        connect();
    }
    
    if (isConnected_) {
        std::string formattedMsg = formatMessage(msg);
        sendData(formattedMsg);
    }
}

void NetworkOutput::flush() {
    // 网络输出通常不需要flush
}

void NetworkOutput::close() {
    std::lock_guard<std::mutex> lock(networkMutex_);
    disconnect();
}

bool NetworkOutput::isAvailable() const {
    std::lock_guard<std::mutex> lock(networkMutex_);
    return isConnected_;
}

bool NetworkOutput::connect() {
    // 这里应该实现实际的网络连接逻辑
    // 暂时只是模拟
    isConnected_ = true;
    return true;
}

void NetworkOutput::disconnect() {
    isConnected_ = false;
}

bool NetworkOutput::isConnected() const {
    std::lock_guard<std::mutex> lock(networkMutex_);
    return isConnected_;
}

bool NetworkOutput::sendData(const std::string& data) {
    // 这里应该实现实际的网络发送逻辑
    // 暂时只是模拟
    return true;
}

std::string NetworkOutput::formatMessage(const LogMessage& msg) {
    std::ostringstream oss;
    oss << "[" << levelToString(msg.level) << "] "
        << std::chrono::duration_cast<std::chrono::seconds>(
               msg.timestamp.time_since_epoch()).count()
        << " " << msg.file << ":" << msg.line;
    
    if (!msg.function.empty()) {
        oss << " " << msg.function;
    }
    
    oss << " - " << msg.message;
    return oss.str();
}

} // namespace async_log
