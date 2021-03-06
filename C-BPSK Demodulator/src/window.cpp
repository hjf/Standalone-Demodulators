#include "window.h"

CBPSKDemodulatorFrame::CBPSKDemodulatorFrame() : wxFrame(NULL, wxID_ANY, "Carrier BPSK Demodulator (by Aang23)")
{
}

bool CBPSKDemodulatorApp::OnInit()
{
    initDSP();

    CBPSKDemodulatorFrame *frame = new CBPSKDemodulatorFrame();
#ifdef __linux__
    frame->SetSize(800, 428);
#elif _WIN32
    frame->SetSize(820, 450);
#elif __APPLE__
    frame->SetSize(820, 450);
#endif

    // Constellation viewer
    drawPane = new ConstellationViewer((wxFrame *)frame);
    drawPane->SetSize(400, 400);

    // Progress stuff
    progressbar = new wxGauge((wxFrame *)frame, 0, 100, wxPoint(400, 390), wxSize(400, 10));
    progressLabel = new wxStaticText((wxFrame *)frame, 0, "Progress : 0%", wxPoint(402, 370));

    // Input / Output file stuff
    selectInputButton = new wxButton((wxFrame *)frame, 1, _T("Input File"), wxPoint(412, 295), wxDefaultSize, 0);
    selectOutputButton = new wxButton((wxFrame *)frame, 2, _T("Output File"), wxPoint(412, 330), wxDefaultSize, 0);
    inputLabel = new wxStaticText((wxFrame *)frame, 0, "No input file selected", wxPoint(502, 305));
    outputLabel = new wxStaticText((wxFrame *)frame, 0, "No output file selected", wxPoint(502, 340));

    // Symbolrate / Symbolrate entries
    samplerateLabel = new wxStaticText((wxFrame *)frame, 0, "Samplerate (Hz) :", wxPoint(412, 15));
    samplerateEntry = new wxTextCtrl((wxFrame *)frame, 0, "3000000", wxPoint(532, 10));
    symbolrateLabel = new wxStaticText((wxFrame *)frame, 0, "Symbolrate (Hz) :", wxPoint(412, 45));
    symbolrateEntry = new wxTextCtrl((wxFrame *)frame, 0, "665400", wxPoint(532, 40));
    rrcAlphaLabel = new wxStaticText((wxFrame *)frame, 0, "RRC Alpha :", wxPoint(412, 75));
    rrcAlphaEntry = new wxTextCtrl((wxFrame *)frame, 0, "0.5", wxPoint(532, 70));
    rrcTapsLabel = new wxStaticText((wxFrame *)frame, 0, "RRC Taps :", wxPoint(412, 105));
    rrcTapsEntry = new wxTextCtrl((wxFrame *)frame, 0, "361", wxPoint(532, 100));

    // Hard symbol option
    noaaDeframerOption = new wxCheckBox((wxFrame *)frame, 0, "NOAA Deframer", wxPoint(412, 130));
    frontRRCOption = new wxCheckBox((wxFrame *)frame, 0, "RRC Filter", wxPoint(412, 160));

    // Baseband type radio buttons
    basebandTypeLabel = new wxStaticText((wxFrame *)frame, 0, "Baseband type :", wxPoint(412, 190));
    wxFont font = basebandTypeLabel->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    basebandTypeLabel->SetFont(font);
    optionF32 = new wxRadioButton((wxFrame *)frame, 0, "32-bits float", wxPoint(412, 220));
    optionI16 = new wxRadioButton((wxFrame *)frame, 0, "16-bits int", wxPoint(412, 250));
    optionI8 = new wxRadioButton((wxFrame *)frame, 0, "8-bits int", wxPoint(532, 220));
    optionW8 = new wxRadioButton((wxFrame *)frame, 0, "8-bits wav", wxPoint(532, 250));

    // Presets
    presetsLabel = new wxStaticText((wxFrame *)frame, 0, "Presets", wxPoint(680, 7));
    font = presetsLabel->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    presetsLabel->SetFont(font);
    presetNOAAHRPT = new wxButton((wxFrame *)frame, 10, _T("NOAA HRPT"), wxPoint(670, 30), wxDefaultSize, 0);
    presetMeteorHRPT = new wxButton((wxFrame *)frame, 11, _T("METEOR HRPT"), wxPoint(670, 65), wxDefaultSize, 0);

    // Start button
    startButton = new wxButton((wxFrame *)frame, 6, _T("Start"), wxPoint(710, 355), wxDefaultSize, 0);

    // Bind buttons
    Bind(wxEVT_BUTTON, [=](wxCommandEvent &event) {
        if (event.GetId() == 1)
        {
            wxFileDialog saveFileDialog((wxFrame *)frame, _("Select baseband file"), "", "", "Baseband|*", wxFD_OPEN);
            if (saveFileDialog.ShowModal() == wxID_CANCEL)
                return;

            inputFilePath = saveFileDialog.GetPath().ToStdString();
            inputLabel->SetLabelText(inputFilePath);
        }
        else if (event.GetId() == 2)
        {
            wxFileDialog saveFileDialog((wxFrame *)frame, _("Select output symbols file"), "", "", "Hard / Soft symbols|*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
            if (saveFileDialog.ShowModal() == wxID_CANCEL)
                return;

            outputFilePath = saveFileDialog.GetPath().ToStdString();
            outputLabel->SetLabelText(outputFilePath);
        }
        else if (event.GetId() == 10)
        {
            samplerateEntry->SetValue("3000000");
            symbolrateEntry->SetValue("665400");
            rrcAlphaEntry->SetValue("0.5");
            rrcTapsEntry->SetValue("31");
            noaaDeframerOption->SetValue(true);
            frontRRCOption->SetValue(false);
            optionI16->SetValue(true);
            noaaMode = true;
            meteorMode = false;
        }
        else if (event.GetId() == 11)
        {
            samplerateEntry->SetValue("3000000");
            symbolrateEntry->SetValue("665400");
            rrcAlphaEntry->SetValue("0.4");
            rrcTapsEntry->SetValue("31");
            noaaDeframerOption->SetValue(false);
            frontRRCOption->SetValue(true);
            optionI16->SetValue(true);
            noaaMode = false;
            meteorMode = true;
        }
        else if (event.GetId() == 6)
        {
            startDSP();
            startButton->Disable();
        }
    });

    //frame->SetSizer(sizer);
    //frame->SetAutoLayout(true);

    frame->Show(true);

    return true;
}
