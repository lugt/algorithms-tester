#include "common_def.h"
#include "xiaoxiaole.h"

enum SortMode {
  Three_Cross_Retro,
  Three_Cross_Graph
};

ELET *values = NULL;
SortMode current_sortmode = Three_Cross_Retro;

namespace {

  void setUpTracer(const char *configFilePath) {
    auto configYAML = YAML::LoadFile(configFilePath);
    auto config = jaegertracing::Config::parse(configYAML);
    auto tracer = jaegertracing::Tracer::make(
      "diamond-service", config, jaegertracing::logging::consoleLogger());
    opentracing::Tracer::InitGlobal(
      std::static_pointer_cast<opentracing::Tracer>(tracer));
  }

  void tracedInit(const std::unique_ptr<opentracing::Span> &parentSpan,
                  ELET_OFST slen) {
    AS_CHILD_SPAN(span, "initialization", parentSpan);
    std::ostringstream oss;
    oss << "length : " << slen;
    span->SetBaggageItem("length", oss.str());
    ELET_OFST length = slen * slen * 4;
    // Initializing the Array in memory (compare with malloc?)
    values = new ELET[length];

    AS_CHILD_SPAN(randomSpan, "rand-loop", span);
    std::default_random_engine generator;
    std::uniform_int_distribution<ELET> distribution(DISTRIBUTE_MIN, DISTRIBUTE_MAX);
    auto dice = std::bind(distribution, generator);
    for (ELET i = 0; i < length; i++) {
      values[i] = dice();
    }
    printf("Init sample : %d %d %d \n", values[0], values[1], values[2]);
    randomSpan->Finish();
  }

  void tracedReduce(const std::unique_ptr<opentracing::Span> &parentSpan,
                    ELET length) {

    std::string name = "basic-sort";
    IFSORT(Three_Cross_Retro, name = "Three_Cross_Retro")
    IFSORT(Three_Cross_Graph, name = "Three_Cross_Graph")

    AS_CHILD_SPAN(span, name, parentSpan);

    std::ostringstream oss;
    oss << "length : " << length << ", Estimate : " << (log(length) * length);
    span->SetBaggageItem("params", oss.str());
    ELET_OFST track_length = length;

    IFSORT(Three_Cross_Retro, Cross_retro(values, track_length, span);)
    IFSORT(Three_Cross_Graph, Cross_graph(values, track_length, span);)
    span->Finish();
  }

  void tracedLoop(SPTR &parentSpan) {
    // start globla init of 50000
    ELET_OFST length = 10;
    tracedInit(parentSpan, length);
    for(ELET_OFST stepWidth = length; stepWidth <= length; stepWidth += 20) {
      tracedReduce(parentSpan, stepWidth);
    }
    // delete[] values;
  }


  void tracedFunction() {
    auto span = opentracing::Tracer::Global()->StartSpan("Sort-Program");
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
