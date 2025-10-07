include(FetchContent)

message("GLM")
FetchContent_Declare(
        glm
        GIT_REPOSITORY	https://github.com/g-truc/glm.git
)

FetchContent_MakeAvailable(glm)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)