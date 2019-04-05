#include "common_def.h"


enum SortMode {
  Sort_Merge,
  Sort_Quick,
  Sort_Quick3,
  Sort_Selection,
  Sort_Bubble,
  Sort_Insertion,
  Sort_Std,
  Sort_Std_Stable,
  Top_10_Of_1Billion,
  Top_10_Of_1Billion_Buffer,
  Top_10_Of_1Billion_Pck_Buffer_Static,
  Top_10_Of_1Billion_Pck_Buffer_Dynamic,
  Top_10_Of_1Billion_LRU,
  Top_10_Of_1Billion_Heap,
  Top_10_Of_1Billion_Pck_Heap
};

ELET *values = NULL;
SortMode current_sortmode = Top_10_Of_1Billion_Pck_Buffer_Dynamic;

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
                  ELET length) {
    AS_CHILD_SPAN(span, "initialization", parentSpan);
    std::ostringstream oss;
    oss << "length : " << length;
    span->SetBaggageItem("length", oss.str());
    // Initializing the Array in memory (compare with malloc?)
    values = new ELET[length];

    AS_CHILD_SPAN(randomSpan, "rand-loop", span);
    std::default_random_engine generator;
    std::uniform_int_distribution<ELET> distribution(DISTRIBUTE_MIN, DISTRIBUTE_MAX);
    auto dice = std::bind(distribution, generator);
    for (ELET i = 0; i < length; i++) {
      values[i] = dice();
    }
    randomSpan->Finish();
  }

  void tracedReduce(const std::unique_ptr<opentracing::Span> &parentSpan,
                    ELET length) {

    std::string name = "basic-sort";
    IFSORT(Sort_Bubble, name = "Sort_Bubble_Whole_Loop")
    IFSORT(Sort_Insertion, name = "Sort_Insertion_Whole_Loop")
    IFSORT(Sort_Merge, name = "Sort_Merge_Whole_Loop")
    IFSORT(Sort_Quick, name = "Sort_Quick_Whole_Loop")
    IFSORT(Sort_Selection, name = "Sort_Selection_Whole_Loop")
    IFSORT(Sort_Std, name = "Sort_Std_Whole_Loop")
    IFSORT(Sort_Std_Stable, name = "Sort_Std_Stable_Whole_Loop")
    IFSORT(Top_10_Of_1Billion, name = "Top_1B")
    IFSORT(Top_10_Of_1Billion_Buffer, name = "Top_1B_Buffer")
    IFSORT(Top_10_Of_1Billion_Pck_Buffer_Static, name = "Top_1B_Pck_Static")
    IFSORT(Top_10_Of_1Billion_Pck_Buffer_Dynamic, name = "Top_1B_Pck_Dynamic")
    IFSORT(Top_10_Of_1Billion_LRU, name = "Top_1B_LRU")
    IFSORT(Top_10_Of_1Billion_Heap, name = "Top_1B_Heap")
    IFSORT(Top_10_Of_1Billion_Pck_Heap, name = "Top_1B_Precheck_Heap")

    AS_CHILD_SPAN(span, name, parentSpan);

    std::ostringstream oss;
    oss << "length : " << length << ", Estimate : " << (log(length) * length);
    span->SetBaggageItem("params", oss.str());
    ELET_OFST track_length = length;

    IFSORT(Sort_Bubble, tracedBubble(values, track_length, span);)
    IFSORT(Sort_Insertion, tracedInsertion(values, track_length, span);)
    IFSORT(Sort_Merge, tracedMerge(values, track_length, span);)
    IFSORT(Sort_Quick, tracedQuick(values, track_length, span);)
    IFSORT(Sort_Selection, tracedSelection(values, track_length, span);)
    IFSORT(Sort_Std, std::sort(&values[0], &values[track_length]))
    IFSORT(Sort_Std_Stable, std::stable_sort(&values[0], &values[track_length]))
    IFSORT(Top_10_Of_1Billion, tracedTopK(values, track_length, span))
    IFSORT(Top_10_Of_1Billion_Buffer, tracedTopKBuffer(values, track_length, span))
    IFSORT(Top_10_Of_1Billion_Pck_Buffer_Static, tracedTopKPckBufferStatic(values, track_length, span))
    IFSORT(Top_10_Of_1Billion_Pck_Buffer_Dynamic, tracedTopKPckBufferDynamic(values, track_length, span))
    IFSORT(Top_10_Of_1Billion_LRU, tracedTopKLRU(values, track_length, span))
    IFSORT(Top_10_Of_1Billion_Heap, tracedTopKHeap(values, track_length, span))
    IFSORT(Top_10_Of_1Billion_Pck_Heap, tracedTopKPckHeap(values, track_length, span))

    span->Finish();
  }

  void tracedLoop(SPTR &parentSpan) {
    // start globla init of 50000
    ELET_OFST length = 100000 * 10000;
    tracedInit(parentSpan, length);
    for(ELET_OFST stepWidth = 20000 * 10000; stepWidth <= length; stepWidth += (20000 * 10000)) {
      tracedReduce(parentSpan, stepWidth);
    }
    delete[] values;
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
