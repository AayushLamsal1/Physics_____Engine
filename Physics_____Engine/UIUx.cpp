#include "UIUX.hpp"

void initUI(tgui::Gui& gui, sf::RenderWindow& window,
    tgui::Button::Ptr& runPauseBtn,
    tgui::Slider::Ptr& gridSlider,
    tgui::Button::Ptr& box0Btn,
    tgui::Button::Ptr& box1Btn,
    tgui::Button::Ptr& box2Btn,
    tgui::Button::Ptr& resetBtn) // new button
{
    const tgui::Color panelColor(201, 190, 212);
    const tgui::Color accent(100, 106, 148);
    const tgui::Color buttonFace(100, 102, 184);
    const tgui::Color textColor(230, 235, 240);

    auto leftPanel = tgui::Panel::create({ 180.f, 180.f });
    leftPanel->setPosition(20.f, 20.f);
    leftPanel->getRenderer()->setBackgroundColor(panelColor);
    leftPanel->getRenderer()->setBorderColor(tgui::Color(10, 18, 28));
    leftPanel->getRenderer()->setRoundedBorderRadius(8);
    gui.add(leftPanel);

    auto leftPanel2 = tgui::Panel::create({ 200.f, 305.f });
    leftPanel2->setPosition(20.f, 220.f);  // 20px below first panel
    leftPanel2->getRenderer()->setBackgroundColor(tgui::Color(0, 0, 0));
    leftPanel2->getRenderer()->setBorderColor(panelColor);
    leftPanel2->getRenderer()->setRoundedBorderRadius(8);
    gui.add(leftPanel2);

    const float boxSize = 60.f;
    const int totalRows = 7;
    const int totalCols = 2;
    const float gap = 8.f;
    int boxCount = 0;

    for (int row = 0; row < totalRows; ++row)
    {
        for (int col = 0; col < totalCols; ++col)
        {
            if (boxCount >= 4) break;

            auto b = tgui::Button::create();
            b->setSize({ boxSize, boxSize });
            b->setPosition({ 20.f + col * (boxSize + gap), 20.f + row * (boxSize + gap) });
            b->setText("");
            b->getRenderer()->setBackgroundColor(buttonFace);
            b->getRenderer()->setBorderColor(tgui::Color(8, 16, 26));
            b->getRenderer()->setRoundedBorderRadius(6);
            leftPanel->add(b);

            if (boxCount == 0)
            {
                auto pic = tgui::Picture::create("C:\\E drive\\Subjects 2nd sem\\OPP\\shapes.png");
                pic->setSize({ boxSize - 12.f, boxSize - 12.f });
                pic->setPosition(b->getPosition() + tgui::Layout2d{ 6.f, 6.f });
                leftPanel->add(pic);

                auto tip = tgui::Label::create("Objects");
                tip->setTextSize(14);
                tip->getRenderer()->setTextColor(textColor);
                pic->setToolTip(tip);

                box0Btn = b;
            }

            if (boxCount == 1)
            {
                auto pic = tgui::Picture::create("C:\\E drive\\Subjects 2nd sem\\OPP\\path.png");
                pic->setSize({ boxSize - 12.f, boxSize - 12.f });
                pic->setPosition(b->getPosition() + tgui::Layout2d{ 6.f, 6.f });
                leftPanel->add(pic);

                auto tip = tgui::Label::create("Path Trace");
                tip->setTextSize(14);
                tip->getRenderer()->setTextColor(textColor);
                pic->setToolTip(tip);

                box1Btn = b;
            }

            if (boxCount == 2)
            {
                auto pic = tgui::Picture::create("C:\\E drive\\Subjects 2nd sem\\OPP\\range.png");
                pic->setSize({ boxSize - 12.f, boxSize - 12.f });
                pic->setPosition(b->getPosition() + tgui::Layout2d{ 6.f, 6.f });
                leftPanel->add(pic);

                auto tip = tgui::Label::create("Range Line");
                tip->setTextSize(14);
                tip->getRenderer()->setTextColor(textColor);
                pic->setToolTip(tip);

                box2Btn = b;
            }

            ++boxCount;
        }
    }

    // Smaller Run/Pause button
    runPauseBtn = tgui::Button::create("Run");
    runPauseBtn->setSize({ 100.f, 50.f }); // decreased width
    runPauseBtn->setPosition({ 20.f, 540.f });
    runPauseBtn->getRenderer()->setBackgroundColor(accent);
    runPauseBtn->getRenderer()->setTextColor(textColor);
    runPauseBtn->getRenderer()->setRoundedBorderRadius(10);
    gui.add(runPauseBtn);

    // Reset button next to Run/Pause
    resetBtn = tgui::Button::create("Reset");
    resetBtn->setSize({ 95.f, 50.f });
    resetBtn->setPosition({ 120.f, 540.f }); // beside runPauseBtn
    resetBtn->getRenderer()->setBackgroundColor(accent);
    resetBtn->getRenderer()->setTextColor(textColor);
    resetBtn->getRenderer()->setRoundedBorderRadius(10);
    gui.add(resetBtn);

    auto zoomBtn = tgui::Button::create("Zoom In");
    zoomBtn->setSize({ 95.f, 50.f });
    zoomBtn->setPosition({ 372.f, 18.f });
    zoomBtn->getRenderer()->setBackgroundColor(buttonFace);
    zoomBtn->getRenderer()->setTextColor(textColor);
    zoomBtn->getRenderer()->setRoundedBorderRadius(10);
    gui.add(zoomBtn);

    auto stopBtn = tgui::Button::create("Stop Clock");
    stopBtn->setSize({ 140.f, 44.f });
    stopBtn->setPosition({ 800.f, 18.f });
    stopBtn->getRenderer()->setBackgroundColor(buttonFace);
    stopBtn->getRenderer()->setTextColor(textColor);
    stopBtn->getRenderer()->setRoundedBorderRadius(10);
    gui.add(stopBtn);

    gridSlider = tgui::Slider::create();
    gridSlider->setSize({ 200.f, 18.f });
    gridSlider->setPosition({ 530.f, 32.f });
    gridSlider->setMinimum(0);
    gridSlider->setMaximum(1);
    gridSlider->setValue(1);
    gui.add(gridSlider);
}