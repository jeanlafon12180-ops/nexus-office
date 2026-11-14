#pragma once

#include <string>

namespace nexus::documents {

enum class EngineState {
    Ready,
    Unavailable,
    Error
};

struct EngineResult {
    bool success = false;
    std::string message;
};

class DocumentEngine {
public:
    virtual ~DocumentEngine() = default;

    virtual EngineState state() const noexcept = 0;
    virtual EngineResult initialize() = 0;
    virtual EngineResult openDocument(const std::string &filePath) = 0;
    virtual EngineResult saveDocument() = 0;
    virtual EngineResult closeDocument() = 0;
};

} // namespace nexus::documents
