#include "fonts.h"
#include "Roboto/Roboto-Medium.h"
#include "Roboto/Roboto-Regular.h"
#include "Inter/Inter_18pt-Light.h"
#include "Inter/Inter_18pt-Medium.h"
#include "Inter/Inter_18pt-Regular.h"
#include "Inter/Inter_18pt-SemiBold.h"
#include "Icons/IconsFontAwesome6.h"
#include "Icons/IconsFontAwesome6Brands.h"
#include "Icons/fa-regular-400.h"
#include "Icons/fa-solid-900.h"
#include "Icons/fa-brands-400.h"

void Fonts::InitFonts() {
    ImGuiIO& io = ImGui::GetIO();

    static const ImWchar ranges[] = {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0x1C80, 0x1C88, // Cyrillic Extended-C
        0x4E00, 0x9FFF, // CJK Unified Ideographs
        0xAC00, 0xD7AF, // Hangul Syllables
        0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        0x31F0, 0x31FF, // Katakana Phonetic Extensions
        0xFF00, 0xFFEF, // Half-width characters
        0x0,
    };

    static const ImWchar icon_ranges[] = {
        ICON_MIN_FA, ICON_MAX_FA, 0
    };

    ImFontConfig font_config;
    font_config.OversampleH = font_config.OversampleV = 1;
    font_config.PixelSnapH = true;

    InterSemiBold16 = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 16, &font_config, ranges);
    io.Fonts->AddFontDefault();
    
    InterLight16 = io.Fonts->AddFontFromMemoryTTF(InterLight, sizeof(InterLight), 16, &font_config, ranges);
    InterLight17 = io.Fonts->AddFontFromMemoryTTF(InterLight, sizeof(InterLight), 17, &font_config, ranges);
    InterLight18 = io.Fonts->AddFontFromMemoryTTF(InterLight, sizeof(InterLight), 18, &font_config, ranges);
    InterLight20 = io.Fonts->AddFontFromMemoryTTF(InterLight, sizeof(InterLight), 20, &font_config, ranges);
    InterLight22 = io.Fonts->AddFontFromMemoryTTF(InterLight, sizeof(InterLight), 22, &font_config, ranges);
    InterLight24 = io.Fonts->AddFontFromMemoryTTF(InterLight, sizeof(InterLight), 24, &font_config, ranges);

    InterMedium16 = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 16, &font_config, ranges);
    InterMedium17 = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 17, &font_config, ranges);
    InterMedium18 = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 18, &font_config, ranges);
    InterMedium20 = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 20, &font_config, ranges);
    InterMedium22 = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 22, &font_config, ranges);
    InterMedium24 = io.Fonts->AddFontFromMemoryTTF(InterMedium, sizeof(InterMedium), 24, &font_config, ranges);

    InterRegular16 = io.Fonts->AddFontFromMemoryTTF(InterRegular, sizeof(InterRegular), 16, &font_config, ranges);
    InterRegular17 = io.Fonts->AddFontFromMemoryTTF(InterRegular, sizeof(InterRegular), 17, &font_config, ranges);
    InterRegular18 = io.Fonts->AddFontFromMemoryTTF(InterRegular, sizeof(InterRegular), 18, &font_config, ranges);
    InterRegular20 = io.Fonts->AddFontFromMemoryTTF(InterRegular, sizeof(InterRegular), 20, &font_config, ranges);
    InterRegular22 = io.Fonts->AddFontFromMemoryTTF(InterRegular, sizeof(InterRegular), 22, &font_config, ranges);
    InterRegular24 = io.Fonts->AddFontFromMemoryTTF(InterRegular, sizeof(InterRegular), 24, &font_config, ranges);

    InterSemiBold17 = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 17, &font_config, ranges);
    InterSemiBold18 = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 18, &font_config, ranges);
    InterSemiBold20 = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 20, &font_config, ranges);
    InterSemiBold22 = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 22, &font_config, ranges);
    InterSemiBold24 = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 24, &font_config, ranges);
    InterSemiBold40 = io.Fonts->AddFontFromMemoryTTF(InterSemiBold, sizeof(InterSemiBold), 40, &font_config, ranges);

    ImFontConfig icons_config;
    icons_config.MergeMode = true;

    FontAwesomeRegular = io.Fonts->AddFontFromMemoryTTF(FARegular, sizeof(FARegular), 12, &icons_config, icon_ranges);
    FontAwesomeSolid = io.Fonts->AddFontFromMemoryTTF(FASolid, sizeof(FASolid), 19, &icons_config, icon_ranges);
    FontAwesomeBrands = io.Fonts->AddFontFromMemoryTTF(FABrands, sizeof(FABrands), 19, &icons_config, icon_ranges);

    io.Fonts->Build();
}