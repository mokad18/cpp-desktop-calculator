#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <FL/fl_draw.H>

#include <string>
#include <sstream>
#include <cmath>

// Tokyo Night palette – macros avoid static-init fiasco with FLTK
#define CLR_BG          fl_rgb_color(0x1A, 0x1B, 0x26)
#define CLR_DISPLAY_BG  fl_rgb_color(0x24, 0x28, 0x3B)
#define CLR_DISPLAY_FG  fl_rgb_color(0xC0, 0xCA, 0xF5)
#define CLR_NUM_BG      fl_rgb_color(0x41, 0x48, 0x68)
#define CLR_NUM_FG      fl_rgb_color(0xC0, 0xCA, 0xF5)
#define CLR_OP_BG       fl_rgb_color(0x7A, 0xA2, 0xF7)
#define CLR_OP_FG       fl_rgb_color(0x1A, 0x1B, 0x26)
#define CLR_EQ_BG       fl_rgb_color(0x9E, 0xCE, 0x6A)
#define CLR_EQ_FG       fl_rgb_color(0x1A, 0x1B, 0x26)
#define CLR_CLR_BG      fl_rgb_color(0xF7, 0x76, 0x8E)
#define CLR_CLR_FG      fl_rgb_color(0x1A, 0x1B, 0x26)
#define CLR_ALT_BG      fl_rgb_color(0x56, 0x5F, 0x89)
#define CLR_ALT_FG      fl_rgb_color(0xC0, 0xCA, 0xF5)
#define CLR_FUNC_BG     fl_rgb_color(0xBB, 0x9A, 0xF7)
#define CLR_FUNC_FG     fl_rgb_color(0x1A, 0x1B, 0x26)

/// Tischrechner mit FLTK-GUI. Kapselt Zustand, Rechenlogik und GUI.
class Calculator {
public:
    Calculator() = default;
    int run();

private:
    std::string display_trail = "0";
    std::string current_input = "0";
    double operand1 = 0.0;
    char   pending_op = 0;
    bool   fresh_input = false;
    bool   just_evaluated = false;

    Fl_Window* window  = nullptr;
    Fl_Output* display = nullptr;

    static const char* op_symbol(char op);
    bool apply_pending_operation(double right_operand);
    void reset_state();
    void rebuild_trail();
    void apply_unary(double (*func)(double), bool error_on_negative);

    static Fl_Button* make_btn(int x, int y, int w, int h,
                               const char* label, Fl_Color bg, Fl_Color fg);
    void build_gui();

    static void cb_digit   (Fl_Widget*, void* self);
    static void cb_dot     (Fl_Widget*, void* self);
    static void cb_sign    (Fl_Widget*, void* self);
    static void cb_percent (Fl_Widget*, void* self);
    static void cb_square  (Fl_Widget*, void* self);
    static void cb_sqrt    (Fl_Widget*, void* self);
    static void cb_op      (Fl_Widget*, void* self);
    static void cb_clear   (Fl_Widget*, void* self);
    static void cb_eval    (Fl_Widget*, void* self);

    void on_digit(int digit);
    void on_dot();
    void on_sign();
    void on_percent();
    void on_square();
    void on_sqrt();
    void on_op(char op);
    void on_clear();
    void on_eval();
};

// ── Rechenlogik ────────────────────────────────────────────────────────────

const char* Calculator::op_symbol(char op) {
    switch (op) {
        case '+': return " + ";
        case '-': return " − ";
        case '*': return " × ";
        case '/': return " ÷ ";
        default:  return "";
    }
}

bool Calculator::apply_pending_operation(double right_operand) {
    switch (pending_op) {
        case '+': operand1 += right_operand; return true;
        case '-': operand1 -= right_operand; return true;
        case '*': operand1 *= right_operand; return true;
        case '/':
            if (right_operand == 0.0) return false;
            operand1 /= right_operand;
            return true;
        default: return true;
    }
}

void Calculator::reset_state() {
    display_trail  = "0";
    current_input  = "0";
    operand1       = 0.0;
    pending_op     = 0;
    fresh_input    = false;
    just_evaluated = false;
}

void Calculator::rebuild_trail() {
    if (pending_op != 0) {
        std::ostringstream oss;
        oss << operand1 << op_symbol(pending_op) << current_input;
        display_trail = oss.str();
    } else {
        display_trail = current_input;
    }
    display->value(display_trail.c_str());
}

void Calculator::apply_unary(double (*func)(double), bool error_on_negative) {
    double val = std::stod(current_input);
    if (error_on_negative && val < 0.0) {
        display_trail = "Error";
        display->value(display_trail.c_str());
        pending_op  = 0;
        fresh_input = true;
        return;
    }
    val = func(val);
    std::ostringstream oss;
    oss << val;
    current_input = oss.str();
    rebuild_trail();
}

// ── Instanzmethoden ────────────────────────────────────────────────────────

void Calculator::on_digit(int digit) {
    if (just_evaluated) reset_state();

    if (fresh_input) {
        current_input = std::to_string(digit);
        fresh_input   = false;
    } else {
        if (current_input == "0")
            current_input = std::to_string(digit);
        else
            current_input += std::to_string(digit);
    }
    rebuild_trail();
}

void Calculator::on_dot() {
    if (just_evaluated) reset_state();

    if (fresh_input) {
        current_input = "0.";
        fresh_input   = false;
    } else if (current_input.find('.') == std::string::npos) {
        current_input += ".";
    }
    rebuild_trail();
}

void Calculator::on_sign() {
    if (current_input == "0" || current_input.empty()) return;
    if (current_input[0] == '-')
        current_input.erase(0, 1);
    else
        current_input.insert(0, "-");
    rebuild_trail();
}

void Calculator::on_percent() {
    apply_unary([](double x) { return x / 100.0; }, false);
}

void Calculator::on_square() {
    apply_unary([](double x) { return x * x; }, false);
}

void Calculator::on_sqrt() {
    apply_unary([](double x) { return std::sqrt(x); }, true);
}

void Calculator::on_op(char op) {
    if (just_evaluated) just_evaluated = false;

    if (pending_op != 0 && !fresh_input) {
        double op2 = std::stod(current_input);
        if (!apply_pending_operation(op2)) {
            display_trail = "Error";
            display->value(display_trail.c_str());
            pending_op  = 0;
            fresh_input = true;
            return;
        }
    } else {
        operand1 = std::stod(current_input);
    }

    pending_op  = op;
    fresh_input = true;

    std::ostringstream oss;
    oss << operand1 << op_symbol(op);
    display_trail = oss.str();
    display->value(display_trail.c_str());
}

void Calculator::on_clear() {
    reset_state();
    display->value(display_trail.c_str());
}

void Calculator::on_eval() {
    if (pending_op == 0) return;

    double op2 = std::stod(current_input);
    if (!apply_pending_operation(op2)) {
        display_trail = "Error";
        display->value(display_trail.c_str());
        pending_op  = 0;
        fresh_input = true;
        return;
    }

    std::ostringstream oss;
    oss << operand1;
    display_trail  = oss.str();
    current_input  = display_trail;
    pending_op     = 0;
    fresh_input    = true;
    just_evaluated = true;
    display->value(display_trail.c_str());
}

// ── Statische Callbacks (FLTK → Instanz) ───────────────────────────────────

void Calculator::cb_digit(Fl_Widget* w, void* self) {
    int digit = w->label()[0] - '0';
    static_cast<Calculator*>(self)->on_digit(digit);
}

void Calculator::cb_dot(Fl_Widget*, void* self) {
    static_cast<Calculator*>(self)->on_dot();
}

void Calculator::cb_sign(Fl_Widget*, void* self) {
    static_cast<Calculator*>(self)->on_sign();
}

void Calculator::cb_percent(Fl_Widget*, void* self) {
    static_cast<Calculator*>(self)->on_percent();
}

void Calculator::cb_square(Fl_Widget*, void* self) {
    static_cast<Calculator*>(self)->on_square();
}

void Calculator::cb_sqrt(Fl_Widget*, void* self) {
    static_cast<Calculator*>(self)->on_sqrt();
}

void Calculator::cb_op(Fl_Widget* w, void* self) {
    const auto* lbl = reinterpret_cast<const unsigned char*>(w->label());
    char op;
    if      (lbl[0] == 0xC3 && lbl[1] == 0xB7) op = '/';
    else if (lbl[0] == 0xC3 && lbl[1] == 0x97) op = '*';
    else if (lbl[0] == 0xE2 && lbl[1] == 0x88) op = '-';
    else                                        op = '+';
    static_cast<Calculator*>(self)->on_op(op);
}

void Calculator::cb_clear(Fl_Widget*, void* self) {
    static_cast<Calculator*>(self)->on_clear();
}

void Calculator::cb_eval(Fl_Widget*, void* self) {
    static_cast<Calculator*>(self)->on_eval();
}

// ── GUI-Aufbau ─────────────────────────────────────────────────────────────

Fl_Button* Calculator::make_btn(int x, int y, int w, int h,
                                 const char* label, Fl_Color bg, Fl_Color fg) {
    Fl_Button* btn = new Fl_Button(x, y, w, h, label);
    btn->box(FL_UP_BOX);
    btn->down_box(FL_DOWN_BOX);
    btn->color(bg);
    btn->selection_color(fl_lighter(bg));
    btn->labelcolor(fg);
    btn->labelsize(18);
    btn->labelfont(FL_HELVETICA_BOLD);
    btn->clear_visible_focus();
    return btn;
}

void Calculator::build_gui() {
    constexpr int W      = 420;
    constexpr int H      = 520;
    constexpr int PAD    = 10;
    constexpr int GAP    = 8;
    constexpr int DISP_H = 70;
    constexpr int COLS   = 5;
    constexpr int ROWS   = 5;
    constexpr int BTN_W  = (W - 2 * PAD - (COLS - 1) * GAP) / COLS;
    constexpr int BTN_H  = (H - 2 * PAD - DISP_H - GAP - (ROWS - 1) * GAP) / ROWS;

    window = new Fl_Window(W, H, "Taschenrechner");
    window->color(CLR_BG);
    window->begin();

    display = new Fl_Output(PAD, PAD, W - 2 * PAD, DISP_H);
    display->box(FL_FLAT_BOX);
    display->color(CLR_DISPLAY_BG);
    display->textcolor(CLR_DISPLAY_FG);
    display->textsize(28);
    display->textfont(FL_HELVETICA);
    display->value("0");

    struct BtnDef { const char* label; char kind; };

    const BtnDef layout[ROWS][COLS] = {
        {{"C", 'c'},  {"±", 's'},  {"%", 'p'},  {"÷", 'o'},  {"√x",'r'}},
        {{"7", 'd'},  {"8", 'd'},  {"9", 'd'},  {"×", 'o'},  {"x²",'q'}},
        {{"4", 'd'},  {"5", 'd'},  {"6", 'd'},  {"−", 'o'},  {"",  0 }},
        {{"1", 'd'},  {"2", 'd'},  {"3", 'd'},  {"+", 'o'},  {"",  0 }},
        {{"0", 'd'},  {".", '.'},  {"=", 'e'},  {"",  0 },   {"",  0 }},
    };

    const int grid_y = PAD + DISP_H + GAP;

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            const BtnDef& def = layout[r][c];
            if (def.label[0] == '\0') continue;

            int bx = PAD + c * (BTN_W + GAP);
            int by = grid_y + r * (BTN_H + GAP);
            int bw = BTN_W;
            int bh = BTN_H;

            if (def.kind == 'e') bw = 3 * BTN_W + 2 * GAP;

            Fl_Button* btn = nullptr;

            switch (def.kind) {
                case 'd':
                    btn = make_btn(bx, by, bw, bh, def.label, CLR_NUM_BG, CLR_NUM_FG);
                    btn->callback(cb_digit, this);
                    break;
                case 'o':
                    btn = make_btn(bx, by, bw, bh, def.label, CLR_OP_BG, CLR_OP_FG);
                    btn->callback(cb_op, this);
                    break;
                case 'c':
                    btn = make_btn(bx, by, bw, bh, def.label, CLR_CLR_BG, CLR_CLR_FG);
                    btn->callback(cb_clear, this);
                    break;
                case 's':
                    btn = make_btn(bx, by, bw, bh, def.label, CLR_ALT_BG, CLR_ALT_FG);
                    btn->callback(cb_sign, this);
                    break;
                case 'p':
                    btn = make_btn(bx, by, bw, bh, def.label, CLR_ALT_BG, CLR_ALT_FG);
                    btn->callback(cb_percent, this);
                    break;
                case '.':
                    btn = make_btn(bx, by, bw, bh, def.label, CLR_NUM_BG, CLR_NUM_FG);
                    btn->callback(cb_dot, this);
                    break;
                case 'e':
                    btn = make_btn(bx, by, bw, bh, def.label, CLR_EQ_BG, CLR_EQ_FG);
                    btn->callback(cb_eval, this);
                    break;
                case 'q':
                    btn = make_btn(bx, by, bw, bh, def.label, CLR_FUNC_BG, CLR_FUNC_FG);
                    btn->callback(cb_square, this);
                    break;
                case 'r':
                    btn = make_btn(bx, by, bw, bh, def.label, CLR_FUNC_BG, CLR_FUNC_FG);
                    btn->callback(cb_sqrt, this);
                    break;
            }
        }
    }

    window->end();
}

int Calculator::run() {
    build_gui();
    window->show();
    return Fl::run();
}

int main() {
    Calculator calc;
    return calc.run();
}