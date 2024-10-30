#include <fstream>
#include <iostream>
#include <raylib.h>
#include <string>
#include <vector>
constexpr int font_size = 32;
constexpr int font_width = 14;
constexpr int font_height = 32;
Font roboto_mono{};
struct QuizData {
    std::string question;
    std::string right_answer;
    std::vector<std::string> wrong_answers;
};
struct Quiz {
    std::string question;
    std::vector<std::string> options;
    int answer;
};
// line in rect height
inline int lirh(const std::string &line, const int w) {
    const int line_limit = w / font_width;
    std::vector<int> space_indices;
    space_indices.push_back(-1);
    for (int i = 0; i < line.size(); ++i) {
        if (line[i] == ' ') {
            space_indices.push_back(i);
        }
    }
    space_indices.push_back(line.size());
    int ci = 0, cl = 0;
    for (int i = 0; i < space_indices.size() - 1; ++i) {
        const int word_size = space_indices[i + 1] - space_indices[i];
        if (ci + word_size > line_limit) {
            ci = 0;
            ++cl;
        } else {
            ci += word_size + 1;
        }
    }
    return cl + 1;
}
// draw line in rect
inline void dlir(const std::string &line, const int x, const int y,
                 const int w) {
    const int line_limit = w / font_width;
    std::vector<int> space_indices;
    for (int i = 0; i < line.size(); ++i) {
        if (line[i] == ' ') {
            space_indices.push_back(i);
        }
    }
    space_indices.push_back(line.size());
    int ci = 0, cl = 0;
    for (int i = 0; i < line.size(); ++i) {
        if (line[i] == ' ') {
            for (int j = 0; j < space_indices.size() - 1; ++j) {
                if (space_indices[j] == i &&
                    ci + space_indices[j + 1] - space_indices[j] > line_limit) {
                    ++i;
                    ci = 0;
                    ++cl;
                }
            }
        }
        const Vector2 position =
            Vector2{static_cast<float>(x + ci * font_width),
                    static_cast<float>(y + cl * font_height)};
        DrawTextCodepoint(roboto_mono, line[i], position, font_size, BLACK);
        ++ci;
    }
}
// draw text box
inline int dtb(const std::string &line, const int x, const int y, const int w,
               const int border_size, const Color &border_color,
               const Color &inner_color) {
    const int h = border_size * 2 + lirh(line, w) * font_height;
    DrawRectangle(x, y, w, h, border_color);
    DrawRectangle(x + border_size, y + border_size, w - 2 * border_size,
                  h - 2 * border_size, inner_color);
    dlir(line, x + border_size, y + border_size, w - 2 * border_size);
    return y + h;
}
inline Quiz make_quiz(const QuizData &data) {
    Quiz q{
        data.question,
        std::vector<std::string>(data.wrong_answers.size() + 1),
        -1,
    };
    std::vector<int> available_slots(q.options.size());
    for (int i = 0; i < available_slots.size(); ++i) {
        available_slots[i] = i;
    }
    q.answer = GetRandomValue(0, available_slots.size() - 1);
    q.options[q.answer] = data.right_answer;
    available_slots.erase(std::next(available_slots.begin(), q.answer));
    for (int i = 0; i < data.wrong_answers.size(); ++i) {
        int j = GetRandomValue(0, available_slots.size() - 1);
        q.options[available_slots[j]] = data.wrong_answers[i];
        available_slots.erase(std::next(available_slots.begin(), j));
    }
    return q;
}
inline int draw_quiz(const int x, const int y, const int w, const Quiz &quiz,
                     const Vector2 cursor, const bool checked,
                     const int selected = -1, const bool show_answer = false) {
    constexpr int border_size = 4.;
    int cy =
        dtb(quiz.question, x, y, w, border_size, GRAY, LIGHTGRAY) + border_size;
    if (show_answer) {
        for (int i = 0; i < quiz.options.size(); ++i) {
            Color border_color = i == selected ? MAROON : GRAY;
            Color inner_color = i == selected ? RED : LIGHTGRAY;
            if (i == quiz.answer) {
                border_color = LIME;
                inner_color = GREEN;
            }
            cy = dtb(quiz.options[i], x, cy, w, border_size, border_color,
                     inner_color) +
                 border_size;
        }
        return selected;
    } else {
        int cs = selected;
        for (int i = 0; i < quiz.options.size(); ++i) {
            Color border_color = i == selected ? BLUE : GRAY;
            Color inner_color = i == selected ? SKYBLUE : LIGHTGRAY;
            Rectangle cbox = Rectangle{
                static_cast<float>(x),
                static_cast<float>(cy),
                static_cast<float>(w),
                static_cast<float>(lirh(quiz.options[i], w) * font_height +
                                   2 * border_size),
            };
            if (CheckCollisionPointRec(cursor, cbox)) {
                if (checked) {
                    cs = i;
                }
                border_color = ORANGE;
            }
            cy = dtb(quiz.options[i], x, cy, w, border_size, border_color,
                     inner_color) +
                 border_size;
        }
        return cs;
    }
}
class QuizGuy {
  public:
    QuizGuy(const std::vector<QuizData> &data) {
        _quizzes = std::vector<Quiz>(data.size());
        for (int i = 0; i < _quizzes.size(); ++i) {
            _quizzes[i] = make_quiz(data[i]);
        }
        _selected = std::vector<int>(data.size(), -1);
    }

    inline void draw_current_quiz(const int x, const int y, const int w) {
        _selected[_i] = draw_quiz(x, y, w, _quizzes[_i], GetMousePosition(),
                                  IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
                                  _selected[_i], _show_answer);
    }

    inline int get_i() const { return _i; }
    inline int get_max_i() const { return _quizzes.size(); }
    inline int get_grade() const {
        int grade = 0;
        for (int i = 0; i < _quizzes.size(); ++i) {
            if (_selected[i] == _quizzes[i].answer) {
                ++grade;
            }
        }
        return grade;
    }
    inline void set_sa(const bool sa) { _show_answer = sa; }
    inline bool get_sa() const { return _show_answer; }

    inline void next_quiz() { _i = (_i + 1) % _quizzes.size(); }
    inline void prev_quiz() {
        _i = (_i + _quizzes.size() - 1) % _quizzes.size();
    }

  private:
    std::vector<Quiz> _quizzes;
    std::vector<int> _selected;
    int _i = 0;
    bool _show_answer = false;
};
int main(int argc, char *argv[]) {
    std::vector<QuizData> quiz_data;
    {
        QuizData cq = {};
        std::ifstream quiz_data_txt("./data/quiz.txt");
        std::string line;
        int i = 0;
        while (std::getline(quiz_data_txt, line)) {
            if (line == "" || line == " ") {
                if (i > 0) {
                    quiz_data.push_back(cq);
                }
                cq = {};
                i = 0;
                continue;
            }
            if (i == 0) {
                cq.question = line;
            } else if (i == 1) {
                cq.right_answer = line;
            } else {
                cq.wrong_answers.push_back(line);
            }
            ++i;
        }
    }

    InitWindow(800, 600, "quizzer");
    SetRandomSeed(time(0));
    QuizGuy quiz_guy{quiz_data};
    roboto_mono = LoadFont("./data/RobotoMono-Regular.ttf");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        int cy = 0;
        cy = dtb(TextFormat("%d / %d", quiz_guy.get_i() + 1,
                            quiz_guy.get_max_i()),
                 0, 0, 200 - 4, 4, GRAY, LIGHTGRAY) +
             4;
        dtb("prev", 200, 0, 200 - 4, 4, GRAY, LIGHTGRAY);
        Rectangle pq_toggle_cbox =
            Rectangle{200, 0, 200, static_cast<float>(cy)};
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            CheckCollisionPointRec(GetMousePosition(), pq_toggle_cbox)) {
            quiz_guy.prev_quiz();
        }
        Rectangle nq_toggle_cbox =
            Rectangle{400, 0, 200, static_cast<float>(cy)};
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            CheckCollisionPointRec(GetMousePosition(), nq_toggle_cbox)) {
            quiz_guy.next_quiz();
        }
        dtb("next", 400, 0, 200 - 4, 4, GRAY, LIGHTGRAY);
        dtb("show answer", 600, 0, 200, 4, quiz_guy.get_sa() ? BLUE : GRAY,
            quiz_guy.get_sa() ? SKYBLUE : LIGHTGRAY);
        Rectangle sa_toggle_cbox =
            Rectangle{600, 0, 200, static_cast<float>(cy)};
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            CheckCollisionPointRec(GetMousePosition(), sa_toggle_cbox)) {
            quiz_guy.set_sa(!quiz_guy.get_sa());
        }
        quiz_guy.draw_current_quiz(0, cy, 800);
        if (quiz_guy.get_sa()) {
            dtb(TextFormat("%d / %d", quiz_guy.get_grade(),
                           quiz_guy.get_max_i()),
                600, 600 - 40, 200, 4, GRAY, LIGHTGRAY);
        }
        EndDrawing();
    }
    UnloadFont(roboto_mono);
    CloseWindow();
    return 0;
}
