#include <object.h>

std::vector<std::shared_ptr<Object>> GetArgsVector(const std::shared_ptr<Object> cell) {
    std::vector<std::shared_ptr<Object>> args;

    auto next_cell = cell;
    while (next_cell) {
        if (Is<Cell>(next_cell)) {
            args.push_back((As<Cell>(next_cell))->GetFirst());

            next_cell = (As<Cell>(next_cell))->GetSecond();
        } else {
            args.push_back(next_cell);
            break;
        }
    }

    return args;
}