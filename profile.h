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
