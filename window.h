#pragma once

#include <atomic>
#include <string>

void createWindow();
void windowMessageLoop();
void redrawWindow();
void setTitle(std::string const &title);

extern std::atomic_bool done;