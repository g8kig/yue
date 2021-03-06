// This file is published under public domain.

#include "base/command_line.h"
#include "nativeui/nativeui.h"

#if defined(OS_WIN)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
  base::CommandLine::Init(0, nullptr);
#else
int main(int argc, const char *argv[]) {
  base::CommandLine::Init(argc, argv);
#endif

  // Initialize the global instance of nativeui.
  nu::State state;

  // Create GUI message loop.
  nu::Lifetime lifetime;

  // Create window with default options, and then show it.
  scoped_refptr<nu::Window> window(new nu::Window(nu::Window::Options()));
  window->SetContentView(new nu::Label("Hello world"));
  window->SetContentSize(nu::SizeF(400, 400));
  window->Center();
  window->Activate();

  // Quit when window is closed.
  window->on_close.Connect([](nu::Window*) {
    nu::Lifetime::GetCurrent()->Quit();
  });

  // Enter message loop.
  lifetime.Run();

  return 0;
}
