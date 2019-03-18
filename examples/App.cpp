#include "common_def.h"


enum SortMode {
  Sort_Merge,
  Sort_Quick,
  Sort_Quick3,
  Sort_Selection,
  Sort_Bubble,
  Sort_Insertion,
  Sort_Std,
  Sort_Std_Stable
};

INT32 *values = NULL;
SortMode current_sortmode = Sort_Selection;

namespace {

  void setUpTracer(const char *configFilePath) {
    auto configYAML = YAML::LoadFile(configFilePath);
    auto config = jaegertracing::Config::parse(configYAML);
    auto tracer = jaegertracing::Tracer::make(
      "example-service", config, jaegertracing::logging::consoleLogger());
    opentracing::Tracer::InitGlobal(
      std::static_pointer_cast<opentracing::Tracer>(tracer));
  }

  void tracedInit(const std::unique_ptr<opentracing::Span> &parentSpan,
                  INT32 length) {
    AS_CHILD_SPAN(span, "initialization", parentSpan);
    std::ostringstream oss;
    oss << "length : " << length;
    span->SetBaggageItem("length", oss.str());
    // Initializing the Array in memory (compare with malloc?)
    values = new INT32[length];

    AS_CHILD_SPAN(randomSpan, "rand-loop", span);
    std::default_random_engine generator;
    std::uniform_int_distribution<INT32> distribution(DISTRIBUTE_MIN, DISTRIBUTE_MAX);
    auto dice = std::bind(distribution, generator);
    for (INT32 i = 0; i < length; i++) {
      values[i] = dice();
    }
    randomSpan->Finish();
  }

  void tracedReduce(const std::unique_ptr<opentracing::Span> &parentSpan,
                    INT32 length) {

    std::string name = "basic-sort";
    IFSORT(Sort_Bubble, name = "Sort_Bubble_Whole_Loop")
    IFSORT(Sort_Insertion, name = "Sort_Insertion_Whole_Loop")
    IFSORT(Sort_Merge, name = "Sort_Merge_Whole_Loop")
    IFSORT(Sort_Quick, name = "Sort_Quick_Whole_Loop")
    IFSORT(Sort_Selection, name = "Sort_Selection_Whole_Loop")
    IFSORT(Sort_Std, name = "Sort_Std_Whole_Loop")
    IFSORT(Sort_Std_Stable, name = "Sort_Std_Stable_Whole_Loop")

    AS_CHILD_SPAN(span, name, parentSpan);

    std::ostringstream oss;
    oss << "length : " << length << ", Estimate : " << (log(length) * length);
    span->SetBaggageItem("params", oss.str());
    INT32 track_length = length;

    IFSORT(Sort_Bubble, tracedBubble(values, track_length, span);)
    IFSORT(Sort_Insertion, tracedInsertion(values, track_length, span);)
    IFSORT(Sort_Merge, tracedMerge(values, track_length, span);)
    IFSORT(Sort_Quick, tracedQuick(values, track_length, span);)
    IFSORT(Sort_Selection, tracedSelection(values, track_length, span);)
    IFSORT(Sort_Std, std::sort(&values[0], &values[track_length]))
    IFSORT(Sort_Std_Stable, std::stable_sort(&values[0], &values[track_length]))

    span->Finish();
  }

  void tracedLoop(SPTR &parentSpan) {
    // start globla init of 50000
    INT32 length = 50 * 1000;
    for(INT32 stepWidth = 10 * 1000; stepWidth <= length; stepWidth += (5000)) {
      tracedInit(parentSpan, length);
      tracedReduce(parentSpan, stepWidth);
    }
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
