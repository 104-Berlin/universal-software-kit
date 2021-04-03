#pragma once

namespace Editor {
    namespace Platform {
        extern EVector<EString> OpenFileDialog(const EString& title, const EVector<EString>& filters = {}, const EString& defaultPath = "");
        extern EString SaveFileDialog(const EString& title, const EVector<EString>& allowedEndings = {});
    };
};