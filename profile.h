// mandelbrot -- interactive mandelbrot set explorer
// Copyright (C) 2022 sedsedus
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#pragma once

template <int id, class Callable, typename... Args>
auto CalcExecTimeWrapper(const char *const desc, Callable fun, Args &&...args) -> decltype(fun(args...))
{
    static int ticks = 0;
    static float total_sum = 0;
    static sf::Clock updateClk;

    ticks++;
    clock_t begin;
    begin = clock();
    fun(std::forward<Args>(args)...);
    auto end = clock();
    auto elapsed = ((float)end - begin) / CLOCKS_PER_SEC;
    ;

    total_sum += elapsed;
    if (updateClk.getElapsedTime().asMilliseconds() > 1000) {
        printf("%30s: avg time: %.1f s\n", desc, (total_sum / ticks));
        total_sum = 0;
        ticks = 0;
        updateClk.restart();
    }
}

#define WRAP2(CLASS, METHOD) CalcExecTimeWrapper<__COUNTER__>(#CLASS "." #METHOD, [CLASS]() { CLASS->METHOD(); })

#define CalcExecTime(METHOD) CalcExecTimeWrapper<__COUNTER__>(#METHOD, [this]() { this->METHOD(); })
