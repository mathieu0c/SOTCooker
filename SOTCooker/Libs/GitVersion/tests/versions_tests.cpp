#include <git_version_structs.hpp>

#include <gtest/gtest.h>

#define INTERNAL_DECLARE_VERSION(maj, min, p) \
  gitversion::Version v##maj##_##min##_##p{.major = maj, .minor = min, .patch = p};

TEST(VersionsTests, Comparison) {
  INTERNAL_DECLARE_VERSION(0, 0, 0);
  INTERNAL_DECLARE_VERSION(1, 0, 0);
  INTERNAL_DECLARE_VERSION(0, 1, 0);
  INTERNAL_DECLARE_VERSION(0, 0, 1);
  INTERNAL_DECLARE_VERSION(0, 0, 15);
  INTERNAL_DECLARE_VERSION(0, 10, 10);
  INTERNAL_DECLARE_VERSION(0, 10, 0);
  INTERNAL_DECLARE_VERSION(1, 0, 10);

  EXPECT_TRUE(v1_0_0 >= v0_0_15);
  EXPECT_TRUE(v1_0_10 >= v1_0_0);
  EXPECT_TRUE(v0_10_10 >= v0_10_0);
  EXPECT_TRUE(v0_0_15 >= v0_0_1);
  EXPECT_TRUE(v0_1_0 < v1_0_0);
  EXPECT_TRUE(v0_10_10 >= v0_0_15);
  EXPECT_TRUE(v1_0_10 == v1_0_10);
  EXPECT_TRUE(v1_0_10 >= v1_0_10);
  EXPECT_FALSE(v0_10_10 >= v1_0_10);
}

TEST(VersionsTests, to_string) {
  using Version = gitversion::Version;

  INTERNAL_DECLARE_VERSION(36, 12, 5);

  std::string string_no_tag{to_string(v36_12_5)};
  std::string string_tag{to_string(v36_12_5, true)};

  std::string expected_string_no_tag{"36.12.5"};
  std::string expected_string_tag{"v36.12.5"};

  EXPECT_EQ(string_no_tag, expected_string_no_tag);
  EXPECT_EQ(string_tag, expected_string_tag);
}

TEST(VersionsTests, representation) {
  using namespace gitversion;

  const INTERNAL_DECLARE_VERSION(0, 0, 0);
  const GitInfo kNoReleaseNotModified{.last_known_version = v0_0_0,
                                      .is_release = false,
                                      .is_modified = false,
                                      .current_commit = "f48e2s",
                                      .current_tag = "help"};
  std::string expected_str{"f48e2s ([help] from v0.0.0)"};
  EXPECT_EQ(GetVersionRepresentationString(kNoReleaseNotModified), expected_str);

  const GitInfo kNoReleaseNotModifiedNoTag{.last_known_version = v0_0_0,
                                           .is_release = false,
                                           .is_modified = false,
                                           .current_commit = "f48e2s",
                                           .current_tag = ""};
  expected_str = "f48e2s (from v0.0.0)";
  EXPECT_EQ(GetVersionRepresentationString(kNoReleaseNotModifiedNoTag), expected_str);

  const GitInfo kReleaseNotModified{.last_known_version = v0_0_0,
                                    .is_release = true,
                                    .is_modified = false,
                                    .current_commit = "f48e2s",
                                    .current_tag = to_string(v0_0_0, true)};
  expected_str = "v0.0.0";
  EXPECT_EQ(GetVersionRepresentationString(kReleaseNotModified), expected_str);

  const GitInfo kNoReleaseModified{.last_known_version = v0_0_0,
                                   .is_release = false,
                                   .is_modified = true,
                                   .current_commit = "f48e2s",
                                   .current_tag = "help"};
  expected_str = "f48e2s ([help] from v0.0.0)-modified";
  EXPECT_EQ(GetVersionRepresentationString(kNoReleaseModified), expected_str);

  const GitInfo kNoReleaseModifiedNoTag{.last_known_version = v0_0_0,
                                        .is_release = false,
                                        .is_modified = true,
                                        .current_commit = "f48e2s",
                                        .current_tag = ""};
  expected_str = "f48e2s (from v0.0.0)-modified";
  EXPECT_EQ(GetVersionRepresentationString(kNoReleaseModifiedNoTag), expected_str);

  const GitInfo kReleaseModified{.last_known_version = v0_0_0,
                                 .is_release = true,
                                 .is_modified = true,
                                 .current_commit = "f48e2s",
                                 .current_tag = to_string(v0_0_0, true)};
  expected_str = "v0.0.0-modified";
  EXPECT_EQ(GetVersionRepresentationString(kReleaseModified), expected_str);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}