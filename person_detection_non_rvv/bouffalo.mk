#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

CXXFLAGS += -Ithird_party/flatbuffers/include
CXXFLAGS += -Ithird_party/gemmlowp
CXXFLAGS += -Ithird_party/ruy

CFLAGS += -Ithird_party/flatbuffers/include
CFLAGS += -Ithird_party/gemmlowp
CFLAGS += -Ithird_party/ruy

CPPFLAGS += -Ithird_party/flatbuffers/include
CPPFLAGS += -Ithird_party/gemmlowp
CPPFLAGS += -Ithird_party/ruy

CFLAGS += -DTF_LITE_USE_GLOBAL_CMATH_FUNCTIONS
CFLAGS += -DTF_LITE_USE_GLOBAL_MIN
CFLAGS += -DTF_LITE_USE_GLOBAL_MAX
CFLAGS += -DTF_LITE_STATIC_MEMORY

CXXFLAGS += -DTF_LITE_USE_GLOBAL_CMATH_FUNCTIONS
CXXFLAGS += -DTF_LITE_USE_GLOBAL_MIN
CXXFLAGS += -DTF_LITE_USE_GLOBAL_MAX
CXXFLAGS += -DTF_LITE_STATIC_MEMORY
CXXFLAGS += -DTF_LITE_STRIP_ERROR_STRINGS

CPPFLAGS += -DTF_LITE_STATIC_MEMORY
CXXFLAGS += -fno-threadsafe-statics

# Test Image
#CPPFLAGS += -DRUN_MODEL_ON_TEST_IMAGES
#CFLAGS += -DRUN_MODEL_ON_TEST_IMAGES
#CXXFLAGS += -DRUN_MODEL_ON_TEST_IMAGES