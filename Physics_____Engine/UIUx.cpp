#include "UIUX.hpp"

void initUI(tgui::Gui& gui, sf::RenderWindow& window,
    tgui::Button::Ptr& runPauseBtn,
    tgui::Slider::Ptr& gridSlider,
    tgui::Button::Ptr& box0Btn)
{
    const tgui::Color panelColor(18, 26, 38);
    const tgui::Color accent(74, 106, 148);
    const tgui::Color buttonFace(46, 62, 84);
    const tgui::Color textColor(230, 235, 240);

    auto leftPanel = tgui::Panel::create({ 200.f, 500.f });
    leftPanel->setPosition(20.f, 20.f);
    leftPanel->getRenderer()->setBackgroundColor(panelColor);
    leftPanel->getRenderer()->setBorderColor(tgui::Color(10, 18, 28));
    leftPanel->getRenderer()->setRoundedBorderRadius(8);
    gui.add(leftPanel);

    const float boxSize = 60.f;
    const int totalRows = 7;
    const int totalCols = 2;
    const float gap = 8.f;
    int boxCount = 0;

    for (int row = 0; row < totalRows; ++row)
    {
        for (int col = 0; col < totalCols; ++col)
        {
            if (boxCount >= 14) break;

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

            ++boxCount;
        }
    }

    runPauseBtn = tgui::Button::create("Run");
    runPauseBtn->setSize({ 160.f, 50.f });
    runPauseBtn->setPosition({ 20.f, 540.f });
    runPauseBtn->getRenderer()->setBackgroundColor(accent);
    runPauseBtn->getRenderer()->setTextColor(textColor);
    runPauseBtn->getRenderer()->setRoundedBorderRadius(10);
    gui.add(runPauseBtn);

    auto zoomBtn = tgui::Button::create("Zoom In");
    zoomBtn->setSize({ 120.f, 44.f });
    zoomBtn->setPosition({ 240.f, 18.f });
    zoomBtn->getRenderer()->setBackgroundColor(buttonFace);
    zoomBtn->getRenderer()->setTextColor(textColor);
    zoomBtn->getRenderer()->setRoundedBorderRadius(10);
    gui.add(zoomBtn);

    auto stopBtn = tgui::Button::create("Stop Clock");
    stopBtn->setSize({ 140.f, 44.f });
    stopBtn->setPosition({ 372.f, 18.f });
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
