#include <iostream>
#include <string>

#include <GL/gl3w.h>

#include "graphics/components/movie/recorder.hpp"
#include <imgui.h>

namespace tomovis {

Recorder::Recorder() {}
Recorder::~Recorder() {}

void Recorder::describe() {
    if (ImGui::Button("rec")) {
        start();
    }
    ImGui::SameLine();
    if (ImGui::Button("pause")) {
        recording_ = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("stop")) {
        stop();
    }
}

void Recorder::start() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    width_ = viewport[2];
    height_ = viewport[3];

    std::string cmd = "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s " +
                      std::to_string(width_) + "x" + std::to_string(height_) +
                      " -i - "
                      "-threads 0 -preset slow -y -c:v libx264 -pix_fmt yuv420p -crf 21 -vf "
                      "vflip output.mkv";

    std::cout << "Recording video... \n";

    ffmpeg_ = popen(cmd.c_str(), "w");
    if (!ffmpeg_) {
        std::cout << "Could not open ffmpeg\n";
        return;
    }
    buffer_ = new int[width_ * height_];

    recording_ = true;
}

void Recorder::stop() {
    if (ffmpeg_) {
        pclose(ffmpeg_);
    }

    if (buffer_) {
        delete buffer_;
    }

    std::cout << "Finished recording video... \n";

    recording_ = false;
}

void Recorder::capture() {
    if (!recording_) {
        return;
    }

    if (!ffmpeg_) {
        return;
    }

    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, buffer_);
    fwrite(buffer_, sizeof(int) * width_ * height_, 1, ffmpeg_);
}

} // namespace tomovis
