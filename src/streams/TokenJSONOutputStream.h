#pragma once
#include <streams/BaseTokenOutputStream.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <string>
#include <deque>
#include <iostream>
#include <memory>
#include <variant>

namespace tokenize
{

using OWrapper = rapidjson::OStreamWrapper;
using JSONWriter = rapidjson::Writer<OWrapper>;
using PrettyJSONWriter = rapidjson::PrettyWriter<OWrapper>;
using WriterPtr = std::shared_ptr<JSONWriter>;
using PrettyWriterPtr = std::shared_ptr<PrettyJSONWriter>;

class TokenJSONOutputStream : public BaseTokenOutputStream
{
private:
    OWrapper ows;
    TokenPtr pending;
    // methods of both are not polymorphic for some reason
    std::variant<WriterPtr, PrettyWriterPtr> writer_ptr;
    bool pretty;

protected:
    bool next() override;

public:
    TokenJSONOutputStream(
        std::ostream & os_,
        BaseTokenInputStream & input_,
        bool pretty_=false);
    bool write() override;
    void start() override;
    void finish() override;
    bool eos() const override;
};
}

