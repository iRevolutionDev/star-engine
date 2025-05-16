include(FetchContent)

message("SPDLOG")
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.15.2
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(spdlog)