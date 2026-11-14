#include "../src/document/unavailabledocumentengine.h"

#include <cassert>

int main()
{
    nexus::documents::UnavailableDocumentEngine engine;

    assert(engine.state() == nexus::documents::EngineState::Unavailable);
    assert(!engine.initialize().success);
    assert(!engine.openDocument("sample.odt").success);
    assert(!engine.saveDocument().success);
    assert(!engine.closeDocument().success);

    return 0;
}
