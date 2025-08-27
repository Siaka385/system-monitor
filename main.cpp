/*
 * main.cpp
 * This file contains the main application entry point, GUI setup, and rendering logic.
 * It initializes SDL and Dear ImGui, sets up the application style, and defines the
 * structure and content of the main application windows (System, Memory/Processes, Network).
 */

#include "header.h"
#include <SDL.h>
#include <algorithm>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h> // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h> // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h> // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h> // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE      // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h> // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE        // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h> // Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Sets the Dear ImGui style, including colors, rounding, and other visual properties.
void setStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.09f, 0.15f, 0.22f, 0.97f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    style.WindowRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 5.0f;
    style.TabRounding = 5.0f;
}

// systemWindow, display information for the system monitorization
void systemWindow(const char *id, ImVec2 size, ImVec2 position)
{
    ImGui::Begin(id);
    ImGui::SetWindowSize(id, size);
    ImGui::SetWindowPos(id, position);

    ImGui::Text("General system information, including OS, hostname, and CPU details.");
    ImGui::Separator();

    ImGui::Text("OS: %s", getOsName());
    ImGui::Text("Hostname: %s", getHostName().c_str());
    ImGui::Text("User: %s", getUserName().c_str());
    ImGui::Text("CPU: %s", CPUinfo().c_str());
    ImGui::Text("Tasks: %d", getTotalTasks());

    ImGui::Separator();

    if (ImGui::BeginTabBar("SystemTabs"))
    {
        if (ImGui::BeginTabItem("CPU"))
        {
            static bool animate = true;
            ImGui::Checkbox("Animate", &animate);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle real-time graph animation");

            static float fps = 30.0f;
            ImGui::SliderFloat("FPS", &fps, 1.0f, 60.0f, "%.1f");

            static float scale = 1.0f;
            ImGui::SliderFloat("Y-Scale", &scale, 0.1f, 2.0f, "%.1f");

            static float values[120] = { 0 };
            static double refresh_time = 0.0;
            static CPUStats old_stats;

            if (refresh_time == 0.0) {
                old_stats = getCPUStats();
            }

            if (!animate || refresh_time == 0.0)
                refresh_time = ImGui::GetTime();
            while (refresh_time < ImGui::GetTime())
            {
                CPUStats new_stats = getCPUStats();
                long long total_old = old_stats.user + old_stats.nice + old_stats.system + old_stats.idle + old_stats.iowait + old_stats.irq + old_stats.softirq + old_stats.steal;
                long long total_new = new_stats.user + new_stats.nice + new_stats.system + new_stats.idle + new_stats.iowait + new_stats.irq + new_stats.softirq + new_stats.steal;
                long long idle_old = old_stats.idle + old_stats.iowait;
                long long idle_new = new_stats.idle + new_stats.iowait;

                long long total_diff = total_new - total_old;
                long long idle_diff = idle_new - idle_old;

                float cpu_usage = 0.0f;
                if (total_diff > 0) {
                    cpu_usage = (total_diff - idle_diff) * 100.0f / total_diff;
                }

                for (int i = 0; i < IM_ARRAYSIZE(values) - 1; i++) {
                    values[i] = values[i + 1];
                }
                values[IM_ARRAYSIZE(values) - 1] = cpu_usage;
                old_stats = new_stats;
                refresh_time += 1.0f / fps;
            }
            char overlay[32];
            sprintf(overlay, "CPU: %.2f %%", values[IM_ARRAYSIZE(values) - 1]);
            float plot_height = 200.0f;
            float max_val = 100.0f / scale;

            ImGui::BeginGroup();
            ImGui::Text("%.0f%%", max_val);
            ImGui::Dummy(ImVec2(0.0f, plot_height - 2.0f * ImGui::GetTextLineHeight()));
            ImGui::Text("0%%");
            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
            ImGui::PlotLines("##CPU", values, IM_ARRAYSIZE(values), 0, overlay, 0.0f, max_val, ImVec2(ImGui::GetContentRegionAvail().x - 40, plot_height));
            ImGui::PopStyleColor();
            ImVec2 plot_min = ImGui::GetItemRectMin();
            ImVec2 plot_max = ImGui::GetItemRectMax();
            float value_y = plot_max.y - (plot_max.y - plot_min.y) * (values[IM_ARRAYSIZE(values) - 1] / max_val);
            ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(plot_max.x, value_y), 4.0f, ImColor(255, 255, 255, 255));
            ImGui::Text("Time");
            ImGui::EndGroup();

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Fan"))
        {
            static bool animate = true;
            ImGui::Checkbox("Animate", &animate);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle real-time graph animation");

            static float fps = 30.0f;
            ImGui::SliderFloat("FPS", &fps, 1.0f, 60.0f, "%.1f");

            static float scale = 1.0f;
            ImGui::SliderFloat("Y-Scale", &scale, 0.1f, 2.0f, "%.1f");

            ImGui::Text("Status: Active");
            ImGui::Text("Speed: %d RPM", getFanSpeed());
            ImGui::Text("Level: Auto");

            static float values[120] = { 0 };
            static double refresh_time = 0.0;
            if (!animate || refresh_time == 0.0)
                refresh_time = ImGui::GetTime();
            while (refresh_time < ImGui::GetTime())
            {
                for (int i = 0; i < IM_ARRAYSIZE(values) - 1; i++) {
                    values[i] = values[i + 1];
                }
                values[IM_ARRAYSIZE(values) - 1] = getFanSpeed();
                refresh_time += 1.0f / fps;
            }
            char overlay[32];
            sprintf(overlay, "Fan Speed: %d RPM", (int)values[IM_ARRAYSIZE(values) - 1]);
            float plot_height = 200.0f;
            float max_val = 5000.0f / scale;

            ImGui::BeginGroup();
            ImGui::Text("%.0f", max_val);
            ImGui::Dummy(ImVec2(0.0f, plot_height - 2.0f * ImGui::GetTextLineHeight()));
            ImGui::Text("0");
            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
            ImGui::PlotLines("##Fan", values, IM_ARRAYSIZE(values), 0, overlay, 0.0f, max_val, ImVec2(ImGui::GetContentRegionAvail().x - 40, plot_height));
            ImGui::PopStyleColor();
            ImVec2 plot_min = ImGui::GetItemRectMin();
            ImVec2 plot_max = ImGui::GetItemRectMax();
            float value_y = plot_max.y - (plot_max.y - plot_min.y) * (values[IM_ARRAYSIZE(values) - 1] / max_val);
            ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(plot_max.x, value_y), 4.0f, ImColor(255, 255, 255, 255));
            ImGui::Text("Time");
            ImGui::EndGroup();

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Thermal"))
        {
            static bool animate = true;
            ImGui::Checkbox("Animate", &animate);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Toggle real-time graph animation");

            static float fps = 30.0f;
            ImGui::SliderFloat("FPS", &fps, 1.0f, 60.0f, "%.1f");

            static float scale = 1.0f;
            ImGui::SliderFloat("Y-Scale", &scale, 0.1f, 2.0f, "%.1f");

            static float values[120] = { 0 };
            static double refresh_time = 0.0;
            if (!animate || refresh_time == 0.0)
                refresh_time = ImGui::GetTime();
            while (refresh_time < ImGui::GetTime())
            {
                for (int i = 0; i < IM_ARRAYSIZE(values) - 1; i++) {
                    values[i] = values[i + 1];
                }
                values[IM_ARRAYSIZE(values) - 1] = getThermalInfo();
                refresh_time += 1.0f / fps;
            }
            char overlay[32];
            sprintf(overlay, "Temp: %.2f C", values[IM_ARRAYSIZE(values) - 1]);
            float plot_height = 200.0f;
            float max_val = 100.0f / scale;

            ImGui::BeginGroup();
            ImGui::Text("%.0fC", max_val);
            ImGui::Dummy(ImVec2(0.0f, plot_height - 2.0f * ImGui::GetTextLineHeight()));
            ImGui::Text("0C");
            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();
            ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.5f, 1.0f, 1.0f));
            ImGui::PlotLines("##Thermal", values, IM_ARRAYSIZE(values), 0, overlay, 0.0f, max_val, ImVec2(ImGui::GetContentRegionAvail().x - 40, plot_height));
            ImGui::PopStyleColor();
            ImVec2 plot_min = ImGui::GetItemRectMin();
            ImVec2 plot_max = ImGui::GetItemRectMax();
            float value_y = plot_max.y - (plot_max.y - plot_min.y) * (values[IM_ARRAYSIZE(values) - 1] / max_val);
            ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(plot_max.x, value_y), 4.0f, ImColor(255, 255, 255, 255));
            ImGui::Text("Time");
            ImGui::EndGroup();

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

// memoryProcessesWindow, display information for the memory and processes information
void memoryProcessesWindow(const char *id, ImVec2 size, ImVec2 position)
{
    ImGui::Begin(id);
    ImGui::SetWindowSize(id, size);
    ImGui::SetWindowPos(id, position);

    ImGui::Text("Detailed breakdown of memory usage and a sortable list of running processes.");
    ImGui::Separator();

    MemInfo memInfo = getMemInfo();
    SwapInfo swapInfo = getSwapInfo();
    DiskInfo diskInfo = getDiskInfo("/");

    float ramUsage = (float)(memInfo.total - memInfo.available) / memInfo.total;
    float swapUsage = (float)(swapInfo.total - swapInfo.free) / swapInfo.total;
    float diskUsage = (float)diskInfo.used / diskInfo.free;

    char ramText[64];
    sprintf(ramText, "RAM: %.2f%% %s / %s", ramUsage * 100.0f, formatBytes((memInfo.total - memInfo.available) * 1024).c_str(), formatBytes(memInfo.total * 1024).c_str());
    ImGui::ProgressBar(ramUsage, ImVec2(-1, 0), ramText);

    char swapText[64];
    sprintf(swapText, "Swap: %s / %s", formatBytes((swapInfo.total - swapInfo.free) * 1024).c_str(), formatBytes(swapInfo.total * 1024).c_str());
    ImGui::ProgressBar(swapUsage, ImVec2(-1, 0), swapText);

    char diskText[64];
    sprintf(diskText, "Disk: %.2f%% %s / %s", diskUsage * 100.0f, formatBytes(diskInfo.used).c_str(), formatBytes(diskInfo.total).c_str());
    ImGui::ProgressBar(diskUsage, ImVec2(-1, 0), diskText);

    if (ImGui::BeginTable("Disk", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Filesystem");
        ImGui::TableSetupColumn("Size");
        ImGui::TableSetupColumn("Used");
        ImGui::TableSetupColumn("Avail");
        ImGui::TableSetupColumn("Use%");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("/");
        ImGui::TableNextColumn();
        ImGui::Text("%s", formatBytes(diskInfo.total).c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%s", formatBytes(diskInfo.used).c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%s", formatBytes(diskInfo.free).c_str());
        ImGui::TableNextColumn();
        ImGui::Text("%.2f%%", diskUsage * 100.0f);

        ImGui::EndTable();
    }

    ImGui::Separator();

    static char filter[256] = "";
    ImGui::InputText("Search(filter)", filter, sizeof(filter));
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Filter processes by name or PID");

    if (ImGui::BeginTable("Processes", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable)) {
        ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("CPU Usage", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Memory", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        static vector<ProcessInfo> processes;
        static double last_refresh_time = 0.0;
        static map<int, bool> selected_pids; // Map to store selection state

        auto sort_processes = [&](const ImGuiTableSortSpecs* sort_specs) {
            if (sort_specs == nullptr || sort_specs->SpecsCount == 0) {
                return;
            }
            const ImGuiTableColumnSortSpecs* sort_spec = &sort_specs->Specs[0];
            switch (sort_spec->ColumnIndex) {
                case 0: // PID
                    sort(processes.begin(), processes.end(), [sort_spec](const ProcessInfo& a, const ProcessInfo& b) {
                        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? (a.pid < b.pid) : (a.pid > b.pid);
                    });
                    break;
                case 1: // Name
                    sort(processes.begin(), processes.end(), [sort_spec](const ProcessInfo& a, const ProcessInfo& b) {
                        string a_name = a.name;
                        string b_name = b.name;
                        transform(a_name.begin(), a_name.end(), a_name.begin(), ::tolower);
                        transform(b_name.begin(), b_name.end(), b_name.begin(), ::tolower);
                        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? (a_name < b_name) : (a_name > b_name);
                    });
                    break;
                case 2: // State
                    sort(processes.begin(), processes.end(), [sort_spec](const ProcessInfo& a, const ProcessInfo& b) {
                        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? (a.state < b.state) : (a.state > b.state);
                    });
                    break;
                case 3: // CPU Usage
                    sort(processes.begin(), processes.end(), [sort_spec](const ProcessInfo& a, const ProcessInfo& b) {
                        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? (a.cpu_percentage < b.cpu_percentage) : (a.cpu_percentage > b.cpu_percentage);
                    });
                    break;
                case 4: // Memory
                    sort(processes.begin(), processes.end(), [sort_spec](const ProcessInfo& a, const ProcessInfo& b) {
                        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? (a.memory < b.memory) : (a.memory > b.memory);
                    });
                    break;
            }
        };

        ImGuiTableSortSpecs* sort_specs = ImGui::TableGetSortSpecs();
        if (ImGui::GetTime() - last_refresh_time > 3.0 || last_refresh_time == 0.0) {
            last_refresh_time = ImGui::GetTime();
            processes = getProcessInfo();
            // Update is_selected based on stored state
            for (auto& p : processes) {
                if (selected_pids.count(p.pid)) {
                    p.is_selected = selected_pids[p.pid];
                } else {
                    p.is_selected = false;
                }
            }
            if (sort_specs) {
                sort_processes(sort_specs);
            }
        }

        if (sort_specs && sort_specs->SpecsDirty) {
            sort_processes(sort_specs);
            sort_specs->SpecsDirty = false;
        }

        for (auto& p : processes) {
            if (strstr(p.name.c_str(), filter) || strstr(to_string(p.pid).c_str(), filter)) {
                ImGui::TableNextRow();
                if (p.is_selected) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_HeaderActive));
                }

                ImGui::TableNextColumn();
                char label[64];
                sprintf(label, "%d##%d", p.pid, p.pid); // Unique label for selectable
                if (ImGui::Selectable(label, p.is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
                    if (ImGui::GetIO().KeyCtrl) {
                        p.is_selected = !p.is_selected;
                    } else {
                        // Deselect all others
                        for (auto& other_p : processes) {
                            other_p.is_selected = false;
                            selected_pids[other_p.pid] = false;
                        }
                        p.is_selected = true;
                    }
                    selected_pids[p.pid] = p.is_selected; // Update stored state
                }
                ImGui::SameLine(); // Keep the text on the same line as the selectable
                ImGui::Text("%d", p.pid);

                ImGui::TableNextColumn();
                ImGui::Text("%s", p.name.c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%c", p.state);
                ImGui::TableNextColumn();
                ImGui::Text("%.2f%%", p.cpu_percentage);
                ImGui::TableNextColumn();
                ImGui::Text("%.2f%%", (float)p.memory * 100.0f / memInfo.total);
            }
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

// network, display information network information
void networkWindow(const char *id, ImVec2 size, ImVec2 position)
{
    ImGui::Begin(id);
    ImGui::SetWindowSize(id, size);
    ImGui::SetWindowPos(id, position);

    ImGui::Text("This section provides a real-time overview of your network activity, showing the data sent and received by each network connection on your computer.");
    ImGui::Separator();

    if (ImGui::BeginTabBar("NetworkTabs"))
    {
        if (ImGui::BeginTabItem("Network Details"))
        {
            vector<InterfaceInfo> interfaces = getNetworkInfo();

            if (ImGui::BeginTable("Network", 18, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Interface");
                ImGui::TableSetupColumn("IP Address");
                ImGui::TableSetupColumn("RX Bytes");
                ImGui::TableSetupColumn("RX Packets");
                ImGui::TableSetupColumn("RX Errs");
                ImGui::TableSetupColumn("RX Drop");
                ImGui::TableSetupColumn("RX Fifo");
                ImGui::TableSetupColumn("RX Frame");
                ImGui::TableSetupColumn("RX Compressed");
                ImGui::TableSetupColumn("RX Multicast");
                ImGui::TableSetupColumn("TX Bytes");
                ImGui::TableSetupColumn("TX Packets");
                ImGui::TableSetupColumn("TX Errs");
                ImGui::TableSetupColumn("TX Drop");
                ImGui::TableSetupColumn("TX Fifo");
                ImGui::TableSetupColumn("TX Colls");
                ImGui::TableSetupColumn("TX Carrier");
                ImGui::TableSetupColumn("TX Compressed");
                ImGui::TableHeadersRow();

                for (const auto& iface : interfaces) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", iface.name.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", iface.ip_address.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", formatBytes(iface.rx_bytes).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.rx_packets);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.rx_errs);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.rx_drop);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.rx_fifo);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.rx_frame);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.rx_compressed);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.rx_multicast);
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", formatBytes(iface.tx_bytes).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.tx_packets);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.tx_errs);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.tx_drop);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.tx_fifo);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.tx_colls);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.tx_carrier);
                    ImGui::TableNextColumn();
                    ImGui::Text("%lld", iface.tx_compressed);
                }

                ImGui::EndTable();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Data Usage"))
        {
            vector<InterfaceInfo> interfaces = getNetworkInfo();
            const long long max_bytes = 2LL * 1024 * 1024 * 1024; // 2GB

            for (const auto& iface : interfaces) {
                // RX Progress Bar
                float rx_usage = 0.0f;
                if (iface.rx_bytes > 0) {
                    rx_usage = (float)iface.rx_bytes / max_bytes;
                }
                char rxUsageText[128];
                sprintf(rxUsageText, "%s RX: %s (0-2GB)", iface.name.c_str(), formatBytes(iface.rx_bytes).c_str());
                ImGui::ProgressBar(rx_usage, ImVec2(-1, 0), rxUsageText);

                // TX Progress Bar
                float tx_usage = 0.0f;
                if (iface.tx_bytes > 0) {
                    tx_usage = (float)iface.tx_bytes / max_bytes;
                }
                char txUsageText[128];
                sprintf(txUsageText, "%s TX: %s (0-2GB)", iface.name.c_str(), formatBytes(iface.tx_bytes).c_str());
                ImGui::ProgressBar(tx_usage, ImVec2(-1, 0), txUsageText);
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

// Main code
int main(int, char **)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window = SDL_CreateWindow("System Monitor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char *name) { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // render bindings
    ImGuiIO &io = ImGui::GetIO();

    // Setup Dear ImGui style
    setStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // background color
    // note : you are free to change the style of the application
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        {
            ImVec2 mainDisplay = io.DisplaySize;
            memoryProcessesWindow("== Memory and Processes ==",
                                  ImVec2((mainDisplay.x / 2) - 20, (mainDisplay.y / 2) + 30),
                                  ImVec2((mainDisplay.x / 2) + 10, 10));
            // --------------------------------------
            systemWindow("== System Monitor ==",
                         ImVec2((mainDisplay.x / 2) - 10, (mainDisplay.y / 2) + 30),
                         ImVec2(10, 10));
            // --------------------------------------
            networkWindow("== Network ==",
                          ImVec2(mainDisplay.x - 20, (mainDisplay.y / 2) - 60),
                          ImVec2(10, (mainDisplay.y / 2) + 50));
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
