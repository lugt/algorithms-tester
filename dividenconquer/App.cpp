#include "common_def.h"

ELET *values = NULL;

/**
 * Choosing which model to use from
 */
WorkingModule current_sortmode = NP_Brute;

namespace {

  void setUpTracer(const char *configFilePath) {
    auto configYAML = YAML::LoadFile(configFilePath);
    auto config = jaegertracing::Config::parse(configYAML);
    auto tracer = jaegertracing::Tracer::make(
      "divide-conquer", config, jaegertracing::logging::consoleLogger());
    opentracing::Tracer::InitGlobal(
      std::static_pointer_cast<opentracing::Tracer>(tracer));
  }

  void tracedInit(const std::unique_ptr<opentracing::Span> &parentSpan,
                  ELET_OFST length) {

    ELET_OFST real_len = length * 2;
    Is_True(length > 0, << "length must be greater than 0, given : " << length);

    AS_CHILD_SPAN(span, "initialization", parentSpan);
    std::ostringstream oss;
    oss << "length : " << real_len;
    span->SetBaggageItem("length", oss.str());
    // Initializing the Array in memory (compare with malloc?)
    values = new ELET[real_len];

    AS_CHILD_SPAN(randomSpan, "rand-loop", span);
    std::default_random_engine generator;
    std::uniform_int_distribution<ELET> distribution(DISTRIBUTE_MIN, DISTRIBUTE_MAX);
    auto dice = std::bind(distribution, generator);
    for (ELET i = 0; i < real_len; i++) {
      values[i] = dice();
    }
    randomSpan->Finish();
  }

  void tracedReduce(const std::unique_ptr<opentracing::Span> &parentSpan,
                    ELET length) {

    std::string name = "basic-divide&conquer";
    IFSORT(NP_Brute, name = "NP_Brute")
    IFSORT(NP_Div_Axis, name = "NP_Div_Axis")
    IFSORT(NP_Div_XYSpit, name = "NP_Div_XYSpit")
    IFSORT(NP_Div_Circle, name = "NP_Div_Circle")
    IFSORT(NP_Div_Elliptic, name = "NP_Div_Elliptic")
    IFSORT(NP_Div_Vectors, name = "NP_Div_Vectors")

    AS_CHILD_SPAN(span, name, parentSpan);

    std::ostringstream oss;
    oss << "length : " << length << ", Estimate : " << (log(length) * length);
    span->SetBaggageItem("params", oss.str());
    ELET_OFST track_length = length;

    IFSORT(NP_Brute, tracedBrute(values, track_length, span);)
    IFSORT(NP_Div_Axis, tracedDivAxis(values, track_length, span);)
    IFSORT(NP_Div_XYSpit, tracedDivXYSpit(values, track_length, span);)
    IFSORT(NP_Div_Circle, tracedDivCircle(values, track_length, span);)
    IFSORT(NP_Div_Elliptic, tracedDivElliptic(values, track_length, span);)
    IFSORT(NP_Div_Vectors, tracedDivVectors(values, track_length, span);)

    span->Finish();
  }

  void tracedLoop(SPTR &parentSpan) {
    // start globla init of 50000
    ELET_OFST length = 100 * 10000;
    tracedInit(parentSpan, length);
    for(ELET_OFST stepWidth = 10 * 10000; stepWidth <= length; stepWidth += (10 * 10000)) {
      tracedReduce(parentSpan, stepWidth);
    }
    delete[] values;
  }


  void tracedFunction() {
    auto span = opentracing::Tracer::Global()->StartSpan("Divide-Conquer");
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
