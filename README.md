<p align="center">
  <img src="http://gtahub.kntech.co/gtahub.png" alt="GTAHUB Logo" width="200"/>
</p>

<h1 align="center">CustomLoadScreen</h1>

<p align="center">
  Mod thay thế màn hình loading mặc định của GTA San Andreas bằng màn hình tùy chỉnh sử dụng Direct3D 9.
</p>

<p align="center">
  <img src="CustomLoadScreen.png" alt="Preview" width="600"/>
</p>

---

## 📋 Giới thiệu

**CustomLoadScreen** là một ASI plugin dành cho GTA San Andreas, cho phép thay thế toàn bộ màn hình loading bằng giao diện tùy chỉnh dựa trên **Direct3D 9 (d3d9)**. Plugin render texture, font và menu overlay trực tiếp lên màn hình thông qua hook Present callback.

- Không phụ thuộc Qt — toàn bộ code dùng **C++17 STL** thuần
- Hỗ trợ build với **MinGW 32-bit** hoặc **MSVC**
- Tích hợp CI/CD với GitLab

---

## 🗂️ Cấu trúc

```
CustomLoadScreen/
├── CustomLoadScreen.cpp/h   # Entry point & Present hook
├── loader.cpp/h             # DllMain, hook install, MessageBox
├── CMakeLists.txt           # CMake build system (C++17)
├── .gitlab-ci.yml           # CI build với MXE toolchain
├── d3d9/                    # DirectX 9 rendering layer
│   ├── proxydirectx.*       # IDirect3DDevice9 proxy
│   ├── directx.*            # CDirectX wrapper
│   ├── d3drender.*          # Font & render utilities
│   ├── texture.*            # SRTexture (render-to-texture)
│   ├── color.*              # SRColor helpers
│   └── MenuManager/         # UI widget system
│       ├── NodeMenu.*       # Base widget class
│       ├── Node.*           # Container with scrollbar
│       ├── Menu.*           # Top-level menu window
│       ├── Text.*           # Text label widget
│       ├── ContextMenu.*    # Context menu popup
│       ├── VerticalLayout.* # Auto-layout container
│       ├── Listing.*        # Scrollable list
│       └── CSlider.*        # Slider control
├── llmo/                    # Low-level memory operations
│   ├── ccallhook.*          # x86 call hook
│   ├── cshortasm.*          # x86 assembler helper
│   └── memsafe.*            # Safe memory read/write
├── CGame/                   # GTA SA game structures
│   ├── CPed.*               # Ped (pedestrian) class
│   ├── CVehicle.*           # Vehicle class
│   ├── CCamera.*            # Camera class
│   └── ...
└── sys/
    └── mman.*               # mmap compatibility (Windows)
```

---

## 🔧 Yêu cầu

| Thành phần | Phiên bản |
|---|---|
| CMake | ≥ 3.20 |
| Compiler | MinGW i686-w64 hoặc MSVC (32-bit) |
| DirectX SDK | June 2010 |
| C++ Standard | C++17 |

---

## 🚀 Build

### MinGW 32-bit (khuyến nghị)

```bash
cmake -B build -G "MinGW Makefiles" \
  -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build
```

### MSVC 32-bit

```powershell
cmake -B build -A Win32
cmake --build build --config Release
```

> [!NOTE]
> DirectX SDK (June 2010) phải được cài đặt hoặc set biến môi trường `DXSDK_DIR`.

Output: **`CustomLoadScreen.asi`** trong thư mục gốc của dự án.

---

## 📦 Cài đặt

1. Build hoặc tải file `CustomLoadScreen.asi` từ [Releases](https://github.com/YoungB08/CustomLoadScreen-GTAHUB/releases)
2. Copy `CustomLoadScreen.asi` và `CustomLoadScreen.png` vào thư mục gốc **GTA San Andreas**
3. Cần có ASI Loader (ví dụ: [Silent's ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader))
4. Khởi động game

---

## 🛠️ Tùy chỉnh

Chỉnh sửa `CustomLoadScreen.png` để thay hình nền loading screen (hỗ trợ PNG, JPG, BMP).

Để thay đổi nội dung hiển thị, sửa file [`CustomLoadScreen.cpp`](CustomLoadScreen.cpp):

```cpp
pFont->PrintShadow(5, 5, -1, std::string(PROJECT_NAME) + " by SR_team");
pFont->PrintShadow(5, 20, -1, "Loading: " + std::to_string(11.111111 * g_vars.gameSatate) + "%");
```

---

## 📄 License

Dự án thuộc về **SR_team / GTAHUB**. Mọi quyền được bảo lưu.
