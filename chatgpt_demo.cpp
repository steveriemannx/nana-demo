/**
 *  \file chatgpt_demo.cpp
 *  \brief A simple ChatGPT-like chat dialog app using Nana C++ Library.
 */

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/place.hpp>

#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace nana;

// Simulated AI response generator
class ChatBot
{
public:
    std::string respond(const std::string& user_msg)
    {
        // Simple keyword-based responses for demo purposes
        std::string lower = user_msg;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        if (lower.find("hello") != std::string::npos ||
            lower.find("hi") != std::string::npos ||
            lower.find("hey") != std::string::npos)
        {
            return "Hello! How can I help you today?";
        }
        if (lower.find("how are you") != std::string::npos)
        {
            return "I'm just a demo running inside a Nana app, but I'm doing great! How about you?";
        }
        if (lower.find("name") != std::string::npos)
        {
            return "I'm NanaBot, a simple chat demo built with the Nana C++ GUI library!";
        }
        if (lower.find("nana") != std::string::npos)
        {
            return "Nana is a cross-platform C++ GUI library. It's lightweight, modern, and easy to use. Check out http://nanapro.org for more!";
        }
        if (lower.find("bye") != std::string::npos ||
            lower.find("goodbye") != std::string::npos)
        {
            return "Goodbye! Feel free to come back anytime.";
        }
        if (lower.find("help") != std::string::npos)
        {
            return "I'm a simple demo chatbot. Try saying: hello, ask about Nana, or just type anything!";
        }
        if (lower.find("weather") != std::string::npos)
        {
            return "I can't check real weather yet, but I hear it's always sunny in Nana-land! \xE2\x98\x80\xEF\xB8\x8F";
        }
        if (lower.find("joke") != std::string::npos)
        {
            static const std::vector<std::string> jokes = {
                "Why do C++ programmers prefer dark mode?\nBecause light attracts bugs!",
                "What's a GUI library's favorite music?\nPop-up!",
                "Why did the widget cross the road?\nTo get to the other frame!"
            };
            return jokes[rand() % jokes.size()];
        }

        static const std::vector<std::string> fallbacks = {
            "That's interesting! Tell me more.",
            "I see. What else is on your mind?",
            "Hmm, I'm just a simple demo, but I'd love to learn more about that.",
            "Thanks for sharing! Is there anything else you'd like to chat about?",
            "I'm not sure I fully understand, but I'm here to listen!",
            "That's cool! By the way, try asking me for a joke or about Nana."
        };
        return fallbacks[rand() % fallbacks.size()];
    }
};

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    // ---- Main Window ----
    form fm(API::make_center(650, 550));
    fm.caption("Nana ChatGPT Demo");
    fm.bgcolor(colors::white);

    place fm_place(fm);

    // ---- Conversation Display (read-only, multi-line textbox) ----
    textbox conversation(fm);
    conversation.multi_lines(true);
    conversation.bgcolor(colors::white);

    // ---- Input Area ----
    textbox input(fm);
    input.multi_lines(false);
    input.tip_string("Type your message here...");
    input.bgcolor(colors::white);

    button send_btn(fm, "Send");
    // Nana button is self-drawn (not rendered by native NSButton).
    // Leave default colors for native-like appearance.
    // To make it rounded, the Nana library button.cpp _m_draw_background
    // needs to use round_rectangle() instead of gradual_rectangle().

    // ---- Status Bar ----
    label status(fm, "  Ready");
    status.text_align(align::left);

    // ---- Layout ----
    fm_place.div("vert margin=5 gap=3 <conversation weight=85%> <input weight=6%> <send_btn weight=5%> <status_bar weight=3%>");

    fm_place["conversation"] << conversation;
    fm_place["input"]        << input;
    fm_place["send_btn"]     << send_btn;
    fm_place["status_bar"]   << status;

    fm_place.collocate();

    // ---- Chat Logic ----
    ChatBot bot;

    auto send_message = [&]()
    {
        std::string user_text = input.text();
        // Trim leading/trailing whitespace
        user_text.erase(0, user_text.find_first_not_of(" \t\n\r"));
        user_text.erase(user_text.find_last_not_of(" \t\n\r") + 1);

        if (user_text.empty())
            return;

        // Display user message
        conversation.append("\n", false);
        conversation.append("You:       " + user_text + "\n", false);
        input.reset("");

        status.caption("  Thinking...");

        // Get the bot response
        std::string response = bot.respond(user_text);

        // Display bot response
        conversation.append("NanaBot:   " + response + "\n", false);

        status.caption("  Ready");

        // Scroll to the bottom
        upoint end_pos(conversation.text_line_count() - 1, 0);
        conversation.caret_pos(end_pos);

        // Re-focus the input
        input.focus();
    };

    // Send button click
    send_btn.events().click([&](const arg_click&)
    {
        send_message();
    });

    // Enter key in input textbox triggers send
    // On macOS, Enter may come as key_press with keyboard::enter or keyboard::enter_n
    input.events().key_press([&](const arg_keyboard& arg)
    {
        if (arg.key == keyboard::enter || arg.key == keyboard::enter_n)
        {
            send_message();
        }
    });

    // Also handle via key_char (backup for some macOS configurations)
    input.events().key_char([&](const arg_keyboard& arg)
    {
        if (arg.key == keyboard::enter || arg.key == keyboard::enter_n)
        {
            send_message();
        }
    });

    // ---- Show & Run ----
    fm.show();

    // Welcome messages AFTER show (ensure graphics context is ready)
    conversation.editable(false);
    conversation.append("NanaBot: Hello! I'm NanaBot, a simple ChatGPT-like demo.\n", false);
    conversation.append("NanaBot: Try saying hello, asking about Nana, or requesting a joke!\n", false);
    conversation.append("NanaBot: Type your message below and press Enter or click Send.\n", false);
    conversation.append("\n", false);

    input.focus();

    exec();
}
