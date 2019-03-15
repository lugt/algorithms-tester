#include <iostream>
#include <yaml-cpp/yaml.h>
#include <jaegertracing/Tracer.h>

#include <random>

typedef int INT32;
typedef long long INT64;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;

namespace {

  void setUpTracer(const char *configFilePath) {
    auto configYAML = YAML::LoadFile(configFilePath);
    auto config = jaegertracing::Config::parse(configYAML);
    auto tracer = jaegertracing::Tracer::make(
      "example-service", config, jaegertracing::logging::consoleLogger());
    opentracing::Tracer::InitGlobal(
      std::static_pointer_cast<opentracing::Tracer>(tracer));
  }

  INT32 *values = NULL;

  void tracedInit(const std::unique_ptr<opentracing::Span> &parentSpan,
                  INT32 length) {
    auto span = opentracing::Tracer::Global()->StartSpan(
      "initialization", {opentracing::ChildOf(&parentSpan->context())});
    std::ostringstream oss;
    oss << "length : " << length;
    span->SetBaggageItem("length", oss.str());
    // Initializing the Array in memory (compare with malloc?)
    values = new INT32[length];

    auto randomSpan = opentracing::Tracer::Global()->StartSpan(
      "rand-loop", {opentracing::ChildOf(&span->context())});
    std::default_random_engine generator;
    std::uniform_int_distribution<INT32> distribution(1, 99999999);
    auto dice = std::bind(distribution, generator);
    for (INT32 i = 0; i < length; i++) {
      values[i] = dice();
    }
    randomSpan->Finish();
  }

  void tracedReduce(const std::unique_ptr<opentracing::Span> &parentSpan,
                    INT32 length);

  void tracedLoop(const std::unique_ptr<opentracing::Span> &parentSpan) {
    // start globla init of 50000
    INT32 length = 10000000;
    tracedInit(parentSpan, length);
    tracedReduce(parentSpan, length);
  }

  void tracedReduce(const std::unique_ptr<opentracing::Span> &parentSpan,
                    INT32 length) {
    auto span = opentracing::v2::Tracer::Global()->StartSpan(
      "Merge-Sort-outer-loop", {ChildOf(&parentSpan->context())});
    INT32 start = 0;
    INT32 track_length = length;
    for (INT32 i = 0; i < track_length; i++) {
      auto inner_span = opentracing::v2::Tracer::Global()->StartSpan(
        "Merge-Sort-inner-loop", {ChildOf(&span->context())});
    }
  }


  void tracedFunction() {
    auto span = opentracing::Tracer::Global()->StartSpan("Merge-Sort");
    tracedLoop(span);
  }

}  // anonymous namespace

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <config-yaml-path>\n";
    return 1;
  }
  setUpTracer(argv[1]);
  tracedFunction();
  // Not stricly necessary to close tracer, but might flush any buffered
  // spans. See more details in opentracing::Tracer::Close() documentation.
  opentracing::Tracer::Global()->Close();
  return 0;
}
