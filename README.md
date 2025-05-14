# Windows-ASR

[![Platform: Windows](https://img.shields.io/badge/platform-Windows-blue.svg)](https://github.com/bookbot-kids/windows-asr) [![C++17](https://img.shields.io/badge/C++-17-green.svg)](#) [![License: Apache 2.0](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](LICENSE)

A powerful, C++-based Automatic Speech Recognition (ASR) library for Windows, complete with console and GUI sample applications. Effortlessly integrate real-time voice commands and transcription into your projects.

---

## 📖 Table of Contents

* [🌟 Features](#-features)
* [🎬 Demo](#-demo)
* [🏗️ Architecture](#️-architecture)
* [⚙️ Prerequisites](#️-prerequisites)
* [🚀 Installation](#-installation)
* [💻 Usage](#-usage)

  * [Console Demo (TestASR)](#console-demo-testasr)
  * [GUI Demo (Windows-ASR)](#gui-demo-windows-asr)
* [📚 API Reference](#-api-reference)
* [🛠️ Extending the Library](#️-extending-the-library)
* [📝 Roadmap](#-roadmap)
* [🤝 Contributing](#-contributing)
* [📄 License](#-license)
* [🙏 Acknowledgements](#-acknowledgements)
* [📬 Contact](#-contact)

---

## 🌟 Features

* **Continuous, Low-Latency Recognition**: Real-time transcription with minimal delay.
* **Event-Driven API**: Receive callbacks for recognized phrases, errors, and state changes.
* **UTF-8 & Multi-Language Support**: Leverage SAPI’s language packs for international applications.
* **Thread-Safe**: Safe to use across multiple threads in your application.
* **Console & GUI Samples**: Reference implementations to get started instantly.

---

## 🎬 Demo

<p align="center">
  <img src="docs/screenshot_console.png" alt="Console Demo" width="45%" />
  <img src="docs/screenshot_gui.png" alt="GUI Demo" width="45%" />
</p>

Try it yourself:

1. Build the solution (see [Installation](#-installation)).
2. Run the console demo or launch the GUI app.

---

## 🏗️ Architecture

The library is organized into three layers:

1. **Core**: Wraps Windows Speech API (SAPI) with `SpeechRecognizer` class.
2. **Examples**:

   * `TestASR`: Console interface.
   * `Windows-ASR`: GUI sample using Win32/MFC.
3. **Documentation & Assets**: UML diagrams, API specs in `docs/`.

> **Diagram**: See `docs/architecture.png` for a visual overview.

---

## ⚙️ Prerequisites

* **OS**: Windows 7 or later
* **IDE**: Visual Studio 2015+ (Desktop C++ workload)
* **SDKs**:

  * Windows SDK 10.x or later
  * Windows Speech Platform SDK (for additional languages)
* **Language Standard**: C++17 support

---

## 🚀 Installation

```bash
git clone https://github.com/bookbot-kids/windows-asr.git
cd windows-asr
# Open windows-asr.sln in Visual Studio
```

1. Configure project include/lib paths to point at your Windows SDK & Speech Platform SDK.
2. Select **Release** (or **Debug**) and **Build Solution**.

---

## 💻 Usage

### Console Demo (TestASR)

```powershell
cd TestASR\bin\Release
TestASR.exe
```

Speak into your default microphone; recognized text prints in real time.

### GUI Demo (Windows-ASR)

```powershell
cd windows-asr\bin\Release
Windows-ASR.exe
```

Interact with a simple UI to start/stop recognition and view transcripts.

---

## 📚 API Reference

```cpp
#include "SpeechRecognizer.h"

int main() {
    SpeechRecognizer recognizer;
    recognizer.Initialize();
    recognizer.StartRecognition();

    // Attach event handlers
    recognizer.OnRecognized([](const std::string &text) {
        std::cout << "Recognized: " << text << std::endl;
    });

    // Keep the app running until user exit
    std::this_thread::sleep_for(std::chrono::minutes(5));

    recognizer.StopRecognition();
}
```

* **Initialize()**: Prepare the recognizer and load grammars.
* **StartRecognition() / StopRecognition()**: Control ASR engine.
* **OnRecognized(callback)**: Register callback for recognized text.

Detailed class and method docs in [`docs/API.md`](docs/API.md).

---

## 🛠️ Extending the Library

* **Custom Grammars**: Load XML grammar files via `LoadGrammarFromFile(path)`.
* **Audio Input**: Integrate custom audio streams by subclassing `IAudioStream`.
* **Threading Models**: Use your own message loop or integrate with GUI frameworks.

---

## 📝 Roadmap

* [ ] Grammar-based command parsing module
* [ ] Microphone selection & audio device settings
* [ ] Support for cloud-based ASR fallback
* [ ] Performance optimizations for large vocabularies

Contributions are welcome—see [Contributing](#-contributing).

---

## 🤝 Contributing

1. Fork the repo
2. Create a feature branch (`git checkout -b feature/YourFeature`)
3. Commit your changes (`git commit -m "feat: Add awesome feature"`)
4. Push to the branch (`git push origin feature/YourFeature`)
5. Open a Pull Request

Please follow our [Code of Conduct](CODE_OF_CONDUCT.md) and coding style guide in `CONTRIBUTING.md`.

---

## 📄 License

This project is licensed under the Apache License 2.0. See [LICENSE](LICENSE) for details.

---

## 🙏 Acknowledgements

* Contributors and community members in Issues & PRs

---

## 📬 Contact

For questions or support, reach out to:

* **Email**: [bookbot-kids@example.com](mailto:bookbot-kids@example.com)
* **Telegram**: [@bookbot-kids on our workspace](https://t.me/super_devops)
* **Issues**: [https://github.com/bookbot-kids/windows-asr/issues](https://github.com/bookbot-kids/windows-asr/issues)
