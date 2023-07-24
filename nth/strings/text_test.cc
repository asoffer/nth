#include "nth/strings/text.h"

#include "nth/test/test.h"

namespace nth {

NTH_TEST("GreedyLineBreak/ascii-empty") {
  {
    std::string_view s        = "";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 0, text_encoding::ascii);
    NTH_EXPECT(result == "");
    NTH_EXPECT(s == "");
    NTH_EXPECT(result.data() == original.data());
    NTH_EXPECT(s.data() == original.data());
  }

  {
    std::string_view s        = "";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 5, text_encoding::ascii);
    NTH_EXPECT(result == "");
    NTH_EXPECT(s == "");
    NTH_EXPECT(result.data() == original.data());
    NTH_EXPECT(s.data() == original.data());
  }
}

NTH_TEST("GreedyLineBreak/ascii-just-whitespace") {
  {
    std::string_view s        = "  \t \n\r ";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 0, text_encoding::ascii);
    NTH_EXPECT(result == "");
    NTH_EXPECT(s == "");
    NTH_EXPECT(result.data() == original.end());
    NTH_EXPECT(s.data() == original.end());
  }

  {
    std::string_view s        = "  \t \n\r ";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 5, text_encoding::ascii);
    NTH_EXPECT(result == "");
    NTH_EXPECT(s == "");
    NTH_EXPECT(result.data() == original.end());
    NTH_EXPECT(s.data() == original.end());
  }
}

NTH_TEST("GreedyLineBreak/ascii-trims-whitespace") {
  {
    std::string_view s        = "  \t \n\r hello";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 4, text_encoding::ascii);
    NTH_EXPECT(result == "hello");
    NTH_EXPECT(s == "");
    NTH_EXPECT(result.data() == &original[7]);
    NTH_EXPECT(s.data() == &original[12]);
  }
  {
    std::string_view s        = "  \t \n\r hello";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 5, text_encoding::ascii);
    NTH_EXPECT(result == "hello");
    NTH_EXPECT(s == "");
    NTH_EXPECT(result.data() == &original[7]);
    NTH_EXPECT(s.data() == original.end());
  }
  {
    std::string_view s        = "  \t \n\r hello";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 6, text_encoding::ascii);
    NTH_EXPECT(result == "hello");
    NTH_EXPECT(s == "");
    NTH_EXPECT(result.data() == &original[7]);
    NTH_EXPECT(s.data() == original.end());
  }
  {
    std::string_view s        = "  \t \n\r hello  ";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 5, text_encoding::ascii);
    NTH_EXPECT(result == "hello");
    NTH_EXPECT(s == "");
    NTH_EXPECT(result.data() == &original[7]);
    NTH_EXPECT(s.data() == original.end());
  }
  {
    std::string_view s        = "  \t \n\r hello  ";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 6, text_encoding::ascii);
    NTH_EXPECT(result == "hello");
    NTH_EXPECT(s == "");
    NTH_EXPECT(result.data() == &original[7]);
    NTH_EXPECT(s.data() == original.end());
  }
}

NTH_TEST("GreedyLineBreak/no-leading-whitespace") {
  {
    std::string_view s        = "hello  there, world!";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 5, text_encoding::ascii);
    NTH_EXPECT(result == "hello");
    NTH_EXPECT(s == "there, world!");
    NTH_EXPECT(result.data() == &original[0]);
    NTH_EXPECT(s.data() == &original[7]);
  }
  {
    std::string_view s        = "hello  there, world!";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 7, text_encoding::ascii);
    NTH_EXPECT(result == "hello");
    NTH_EXPECT(s == "there, world!");
    NTH_EXPECT(result.data() == &original[0]);
    NTH_EXPECT(s.data() == &original[7]);
  }
  {
    std::string_view s        = "hello  there, world!";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 8, text_encoding::ascii);
    NTH_EXPECT(result == "hello");
    NTH_EXPECT(s == "there, world!");
    NTH_EXPECT(result.data() == &original[0]);
    NTH_EXPECT(s.data() == &original[7]);
  }
  {
    std::string_view s        = "hello  there, world!";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 15, text_encoding::ascii);
    NTH_EXPECT(result == "hello  there,");
    NTH_EXPECT(s == "world!");
    NTH_EXPECT(result.data() == &original[0]);
    NTH_EXPECT(s.data() == &original[14]);
  }
}

}  // namespace nth
