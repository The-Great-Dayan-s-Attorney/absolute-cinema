# Absolute Cinema
![image](https://github.com/user-attachments/assets/7930171f-0d0a-4c0b-a579-caf836623a3f)


<br />

<div  align="center">

<a  href="https://github.com/The-Great-Dayan-s-Attorney/absolute-cinema">

</a>

<h3  align="center">Visual Novel CLI Using Tree, Queue, Stack, LinkedList</h3>

<p  align="center">

A CLI-based visual novel system that allows users to play and create branching stories. Each story is composed of sequential chapters, with each chapter containing a tree of scenes shaped by player choices. The program supports loading default stories, creating custom ones, and saving progress.

<br/>

## About The Project

</div>

### Built With

<!-- row images -->
<div>
  <a href="https://flutter.dev/">
    <img src="https://img.shields.io/badge/CLang-20232A?style=for-the-badge&logo=c&logoColor=61DAFB" alt="C Language" />
  </a>
</div>

## Getting Started

1. Clone the repository:

    ```bash
    git clone https://github.com/The-Great-Dayan-s-Attorney/absolute-cinema
    ```

## Features

### Game Modes

- **Creator Mode**: Classic two-player mode
- **Player Mode**: Challenge AI opponents of varying difficulty

### 🎮 Game Features

- 🧩 **Multi-Story Support**  
  Create and manage multiple stories, each with a unique title and description.

- 📚 **Chapter Queue System**  
  Organize chapters within each story using a queue structure. Easily add, edit, view, or delete chapters.

- 🎭 **Scene & Choice-Based Structure**  
  Create scenes with multiple choices. Choices can link to new or existing scenes, allowing for non-linear storytelling.

- 🧠 **Branching Narrative Flow**  
  Visualize and manage how scenes connect. Enables complex story graphs similar to visual novels.

- 💾 **File Persistence**  
  Automatically save and load stories, chapters, scenes, and choices using human-readable text files with delimiter-based formatting.

- 🛠️ **Full Editing Capability**  
  Edit titles and descriptions of stories, chapters, and scenes. Remove chapters or scenes without breaking story integrity.

- 🖥️ **CLI-Based Interface**  
  All features accessible via a clean command-line interface (terminal), suitable for development and testing.

- 🔁 **Dynamic Scene ID System**  
  Each scene is assigned a unique ID for linking and editing purposes, making story structures easier to navigate.

- 🔄 **Game State Management**  
  Supports saving and continuing story progress, including all chapters and scenes.

- ⏪ **Undo/Redo System** *(Coming Soon)*  
  Navigate backward and forward through changes for easy editing recovery.

## 🎮 How to Play

### 🛠️ Masuk ke Menu Build

1. **Jalankan Program**

2. **Masuk ke Menu Build**
   - Pilih `1. Build Game` dari menu utama.
   - Di sini kamu bisa:
     - Menambahkan story baru.
     - Melihat dan memilih story yang ada.
     - Mengedit atau menghapus story.

3. **Pilih atau Buat Story**
   - Buat story dengan judul dan deskripsi.
   - Atau pilih story yang sudah tersimpan dari folder `data/`.

4. **Tambahkan Chapter**
   - Dalam menu story, kamu bisa menambahkan beberapa chapter dengan sistem antrean.
   - Setiap chapter memiliki:
     - Judul dan deskripsi
     - Sekumpulan scene yang membentuk jalannya cerita.

5. **Bangun Scene dan Pilihan**
   - Buat scene pertama lengkap dengan pilihan-pilihan yang akan membawa ke scene lain.
   - Pilihan bisa diarahkan ke:
     - Scene baru yang kamu buat langsung.
     - Scene lain yang sudah ada sebelumnya.

6. **Simulasi & Struktur**
   - Kamu bisa melihat struktur cerita (scene dan hubungan antar scene).
   - Story disimpan ke dalam file teks untuk dilanjutkan atau dimainkan nanti.

---

### ▶️ Masuk ke Menu Play

1. **Pilih `2. Play` di Menu Utama**
   - Masuk ke mode bermain cerita.

2. **Mulai Permainan Baru**
   - Pilih story dan chapter awal yang ingin dimainkan.
   - Permainan dimulai dari scene pertama.

3. **Navigasi Cerita**
   - Setiap scene akan menampilkan deskripsi dan daftar pilihan.
   - Pilih salah satu opsi untuk melanjutkan ke scene tujuan berikutnya.

4. **Lanjutkan Permainan**
   - Jika sebelumnya kamu sudah menyimpan permainan, kamu bisa lanjut dari scene terakhir.

5. **Simpan & Muat Permainan**
   - Permainan dapat disimpan ke file `savegame.dat`.
   - File ini bisa dimuat kembali untuk melanjutkan permainan kapan saja.

6. **Lihat Riwayat**
   - Kamu bisa melihat seluruh pilihan yang pernah kamu ambil selama permainan berjalan.

---

## 📁 Project Structure

```
absolute-cinema/
├── data/                        # Folder penyimpanan story & chapter
│   ├── [Story Name]/           # Tiap folder berisi 1 story
│   │   ├── details_story.txt   # Metadata story
│   │   └── chapter_X.txt       # Chapter dengan format scene dan pilihan
│   └── ...
│
├── saves/                      # File save state permainan
│   └── [Story Name].txt
│
├── src/                        # Kode sumber utama
│   ├── build/                  # Modul pembuatan struktur cerita
│   │   └── (chapter.c, .h ...)
│   ├── chapter.[c/h]
│   ├── choices.h
│   ├── convention.h
│   ├── filemanager.[c/h]
│   ├── game.[c/h]
│   ├── linkedlist.[c/h]
│   ├── main.c
│   ├── menuModeCreate.c
│   ├── queue.[c/h]
│   └── riwayat.c
│
├── main.exe                    # Hasil kompilasi (opsional)
├── .vscode/                    # Pengaturan editor (jika pakai VSCode)
└── .dist/                      # Folder distribusi (opsional)
```



## Created by
- [Dayan Restu Anasta](dayan.restu.tif424@polban.ac.id)
- [Fauzi Ismail](https://github.com/mailvlous)
- [Virli Nasyila Putri](https://github.com/VirliNasyila)
