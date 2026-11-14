#include "unavailabledocumentengine.h"

namespace nexus::documents {
namespace {
const std::string unavailableMessage =
    "The document engine is not connected yet. No document has been opened or changed.";
}

EngineState UnavailableDocumentEngine::state() const noexcept
{
    return EngineState::Unavailable;
}

EngineResult UnavailableDocumentEngine::initialize()
{
    return {false, unavailableMessage};
}

EngineResult UnavailableDocumentEngine::openDocument(const std::string &filePath)
{
    (void)filePath;
    return {false, unavailableMessage};
}

EngineResult UnavailableDocumentEngine::saveDocument()
{
    return {false, unavailableMessage};
}

EngineResult UnavailableDocumentEngine::closeDocument()
{
    return {false, "No document engine session is active."};
}

} // namespace nexus::documents
