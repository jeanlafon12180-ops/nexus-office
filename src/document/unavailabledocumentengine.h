#pragma once

#include "documentengine.h"

namespace nexus::documents {

class UnavailableDocumentEngine final : public DocumentEngine {
public:
    EngineState state() const noexcept override;
    EngineResult initialize() override;
    EngineResult openDocument(const std::string &filePath) override;
    EngineResult saveDocument() override;
    EngineResult closeDocument() override;
};

} // namespace nexus::documents
