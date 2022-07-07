#include "hershey_fonts.hpp"
#include "common/unicode_sorta.hpp"
#include <cmath>

namespace hershey {
  std::map<std::string, const font_t*> fonts = {
    { "sans",         &futural },
    //{ "sans_bold",    &futuram },
    { "gothic",       &gothgbt },
    //{ "cursive_bold", &scriptc },
    { "cursive",      &scripts },
    { "serif_italic", &timesi  },
    { "seri