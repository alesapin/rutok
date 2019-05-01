#pragma once
#include <streams/BaseTokenOutputStream.h>
#include <streams/BaseCharOutputStream.h>
#include <rapidjson/writer.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>
#include <sstream>
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
    std::ostringstream oss;
    // methods of both are not polymorphic for some reason
    std::variant<WriterPtr, PrettyWriterPtr> writer_ptr;
    BaseCharOutputStream & output;
    bool pretty;

protected:
    bool next(const Token * token) override;

public:
    TokenJSONOutputStream(
        BaseCharOutputStream & output_,
        bool pretty_=false);
    void write(const Token * token) override;
    void start() override;
    void finish() override;
    void flush() override;
};
}

