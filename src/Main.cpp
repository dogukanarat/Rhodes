#include <rhodeus/Application.hpp>

int main(int argc, char** argv)
{
    return Rhodeus::Application::getInstance().run(argc, argv);
}
