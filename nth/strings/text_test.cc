#include "nth/strings/text.h"

#include "gtest/gtest.h"

namespace nth {

TEST(GreedyLineBreak, AsciiEmpty) {
  {
    std::string_view s        = "";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 0, text_encoding::ascii);
    EXPECT_EQ(result, "");
    EXPECT_EQ(s, "");
    EXPECT_EQ(result.data(), original.data());
    EXPECT_EQ(s.data(), original.data());
  }

  {
    std::string_view s        = "";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 5, text_encoding::ascii);
    EXPECT_EQ(result, "");
    EXPECT_EQ(s, "");
    EXPECT_EQ(result.data(), original.data());
    EXPECT_EQ(s.data(), original.data());
  }
}

TEST(GreedyLineBreak, AsciiJustWhitespace) {
  {
    std::string_view s        = "  \t \n\r ";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 0, text_encoding::ascii);
    EXPECT_EQ(result, "");
    EXPECT_EQ(s, "");
    EXPECT_EQ(result.data(), original.end());
    EXPECT_EQ(s.data(), original.end());
  }

  {
    std::string_view s        = "  \t \n\r ";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 5, text_encoding::ascii);
    EXPECT_EQ(result, "");
    EXPECT_EQ(s, "");
    EXPECT_EQ(result.data(), original.end());
    EXPECT_EQ(s.data(), original.end());
  }
}

TEST(GreedyLineBreak, AsciiTrimsWhitespace) {
  {
    std::string_view s        = "  \t \n\r hello";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 4, text_encoding::ascii);
    EXPECT_EQ(result, "hello");
    EXPECT_EQ(s, "");
    EXPECT_EQ(result.data(), &original[7]);
    EXPECT_EQ(s.data(), &original[12]);
  }
  {
    std::string_view s        = "  \t \n\r hello";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 5, text_encoding::ascii);
    EXPECT_EQ(result, "hello");
    EXPECT_EQ(s, "");
    EXPECT_EQ(result.data(), &original[7]);
    EXPECT_EQ(s.data(), original.end());
  }
  {
    std::string_view s        = "  \t \n\r hello";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 6, text_encoding::ascii);
    EXPECT_EQ(result, "hello");
    EXPECT_EQ(s, "");
    EXPECT_EQ(result.data(), &original[7]);
    EXPECT_EQ(s.data(), original.end());
  }
  {
    std::string_view s        = "  \t \n\r hello  ";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 5, text_encoding::ascii);
    EXPECT_EQ(result, "hello");
    EXPECT_EQ(s, "");
    EXPECT_EQ(result.data(), &original[7]);
    EXPECT_EQ(s.data(), original.end());
  }
  {
    std::string_view s        = "  \t \n\r hello  ";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 6, text_encoding::ascii);
    EXPECT_EQ(result, "hello");
    EXPECT_EQ(s, "");
    EXPECT_EQ(result.data(), &original[7]);
    EXPECT_EQ(s.data(), original.end());
  }
}

TEST(GreedyLineBreak, NoLeadingWhitespace) {
  {
    std::string_view s        = "hello  there, world!";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 5, text_encoding::ascii);
    EXPECT_EQ(result, "hello");
    EXPECT_EQ(s, "there, world!");
    EXPECT_EQ(result.data(), &original[0]);
    EXPECT_EQ(s.data(), &original[7]);
  }
{
    std::string_view s        = "hello  there, world!";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 7, text_encoding::ascii);
    EXPECT_EQ(result, "hello");
    EXPECT_EQ(s, "there, world!");
    EXPECT_EQ(result.data(), &original[0]);
    EXPECT_EQ(s.data(), &original[7]);
  }
  {
    std::string_view s        = "hello  there, world!";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 8, text_encoding::ascii);
    EXPECT_EQ(result, "hello");
    EXPECT_EQ(s, "there, world!");
    EXPECT_EQ(result.data(), &original[0]);
    EXPECT_EQ(s.data(), &original[7]);
  }
  {
    std::string_view s        = "hello  there, world!";
    std::string_view original = s;
    std::string_view result   = GreedyLineBreak(s, 15, text_encoding::ascii);
    EXPECT_EQ(result, "hello  there,");
    EXPECT_EQ(s, "world!");
    EXPECT_EQ(result.data(), &original[0]);
    EXPECT_EQ(s.data(), &original[14]);
  }
}

}  // namespace nth
