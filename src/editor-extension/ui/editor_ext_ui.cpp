#include "editor_extension.h"

#include "imgui_internal.h"

using namespace Engine;

void intern::InitUI()
{
    Graphics::Wrapper::SetImGuiContext(Graphics::Wrapper::GetCurrentImGuiContext());
    ImGui::SetCurrentContext(Graphics::Wrapper::GetCurrentImGuiContext());
}

EUIField::EUIField(const EString& label) 
    :   fLabel(label), 
        fID(next_ui_id()), 
        fVisible(true), 
        fDirty(false), 
        fIsContextMenuOpen(false),
        fIsTooltipOpen(false),
        fWidthOverride(0),
        fHeightOverride(0),
        fCalculatedSize(),
        fLastMousePos(0.0f, 0.0f),
        fDragType(),
        fDragData()
{
    
}

EWeakRef<EUIField> EUIField::AddChild(const ERef<EUIField>& child) 
{
    if (!child) { return ERef<EUIField>(nullptr); }
    fChildren.push_back(child);
    return fChildren.back();
}

EWeakRef<EUIField> EUIField::GetChildAt(u32 index) const
{
    if (index < fChildren.size())
    {
        return fChildren[index];
    }
    E_WARN("Could not get child of index " + std::to_string(index));
    return ERef<EUIField>(nullptr);
}


EVector<EWeakRef<EUIField>> EUIField::GetChildren() const
{
    EVector<EWeakRef<EUIField>> result;
    for (auto& entry : fChildren)
    {
        result.push_back(entry);
    }
    return result;
}

void EUIField::RemoveChild(const EWeakRef<EUIField>& child) 
{
    if (child.expired()) {return;}
    EVector<ERef<EUIField>>::iterator foundChild = std::find(fChildren.begin(), fChildren.end(), child.lock());
    if (foundChild != fChildren.end())
    {
        fChildren.erase(foundChild);
    }
}

void EUIField::Clear() 
{
    fChildren.clear();
}

void EUIField::SetCustomUpdateFunction(UpdateFunction function) 
{
    fCustomUpdateFunction = function;
}

void EUIField::Render() 
{
    if (!fVisible) { return; }

    if (fDirty && fCustomUpdateFunction)
    {
        fCustomUpdateFunction();
        fDirty = false;
    }
    ImGui::PushID(fID);

    if (OnRender())
    {
        for (ERef<EUIField> uiField : fChildren)
        {
            OnBeforeChildRender();
            uiField->Render();
            OnAfterChildRender();
        }
    }
    
    HandleRenderEndBefore();
    OnRenderEnd();
    HandleRenderEnd();
    ImGui::PopID();
}

bool EUIField::OnRender() 
{
    return true;
}

void EUIField::HandleRenderEndBefore()
{
    if (ImGui::IsItemHovered())
    {
        if (fToolTip)
        {
            fIsTooltipOpen = true;
            ImGui::BeginTooltip();
            fToolTip->Render();
            ImGui::EndTooltip();
        }
    }
    
    if (!fAcceptDragType.empty())
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(fAcceptDragType.c_str()))
            {
                EUIDragData dragData = *(EUIDragData*)payload->Data;
                fEventDispatcher.Enqueue<events::EDropEvent>({fAcceptDragType, dragData.Data_ID, dragData.Data_String});
            }
            ImGui::EndDragDropTarget();
        }
    }

    if (!fDragType.empty())
    {
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {

            ImGui::SetDragDropPayload(fDragType.c_str(), &fDragData, sizeof(EUIDragData));
            ImGui::Text(fDragType.c_str());
            ImGui::EndDragDropSource();
        }
    }
    
}

void EUIField::HandleRenderEnd()
{
    ImGuiContext& g = *Graphics::Wrapper::GetCurrentImGuiContext();

    fIsContextMenuOpen = false;
    if (fContextMenu)
    {
        if (ImGui::BeginPopupContextItem(GetLabel().c_str()))
        {
            fIsContextMenuOpen = true;
            fContextMenu->Render();
            ImGui::EndPopup();
        }
    }


    ImRect itemRect = g.LastItemData.Rect;
    
    fCalculatedSize.x = itemRect.GetWidth();
    fCalculatedSize.y = itemRect.GetHeight();

    fIsTooltipOpen = false;
    if (ImGui::IsItemHovered())
    {
        EVec2 mousePos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
        mousePos -= EVec2(itemRect.Min.x, itemRect.Min.y);

        ImVec2 md0 = ImGui::GetMouseDragDelta(0, 0.0f);
        EVec2 mouseDrag0(md0.x, md0.y);
        ImGui::ResetMouseDragDelta(0);
        ImVec2 md1 = ImGui::GetMouseDragDelta(1, 0.0f);
        EVec2 mouseDrag1(md1.x, md1.y);
        ImGui::ResetMouseDragDelta(1);
        ImVec2 md2 = ImGui::GetMouseDragDelta(2, 0.0f);
        EVec2 mouseDrag2(md2.x, md2.y);
        ImGui::ResetMouseDragDelta(2);


        EVec2 mouseDelta = mousePos - fLastMousePos;
        fLastMousePos = mousePos;
        if (glm::length(mouseDelta) > 0.0f)
        {
            fEventDispatcher.Enqueue<events::EMouseMoveEvent>({mousePos, mouseDelta});
        }
        
        if (ImGui::IsMouseClicked(0))
        {
            fEventDispatcher.Enqueue<events::EMouseDownEvent>(events::EMouseDownEvent{mousePos, 0});
        }
        if (ImGui::IsMouseClicked(1))
        {
            fEventDispatcher.Enqueue<events::EMouseDownEvent>(events::EMouseDownEvent{mousePos, 1});
        }
        if (ImGui::IsMouseClicked(2))
        {
            fEventDispatcher.Enqueue<events::EMouseDownEvent>(events::EMouseDownEvent{mousePos, 2});
        }
        if (glm::length(mouseDrag0) > 0.0f)
        {
            fEventDispatcher.Enqueue<events::EMouseDragEvent>({mousePos, mouseDrag0, 0});
        }
        if (glm::length(mouseDrag1) > 0.0f)
        {
            fEventDispatcher.Enqueue<events::EMouseDragEvent>({mousePos, mouseDrag1, 1});
        }
        if (glm::length(mouseDrag2) > 0.0f)
        {
            fEventDispatcher.Enqueue<events::EMouseDragEvent>({mousePos, mouseDrag2, 2});
        }
    }    
}

void EUIField::OnRenderEnd() 
{
    
}

void EUIField::UpdateEventDispatcher() 
{
    OnUpdateEventDispatcher();
    fEventDispatcher.Update();

    for (ERef<EUIField> child : fChildren)
    {
        child->UpdateEventDispatcher();
    }

    if (fIsContextMenuOpen)
    {
        fContextMenu->UpdateEventDispatcher();
    }
    if (fIsTooltipOpen)
    {
        fToolTip->UpdateEventDispatcher();
    }
}

void EUIField::DisconnectAllEvents() 
{
    fEventDispatcher.DisconnectEvents();
    for (ERef<EUIField> child : fChildren)
    {
        child->DisconnectAllEvents();
    }
}

const EString& EUIField::GetLabel() const
{
    return fLabel;    
}

void EUIField::SetSize(float width, float height) 
{
    fWidthOverride = width;
    fHeightOverride = height;
}

void EUIField::SetSize(const EVec2& size) 
{
    fWidthOverride = size.x;
    fHeightOverride = size.y;
}

void EUIField::SetWidth(float width) 
{
    fWidthOverride = width;
}

void EUIField::SetHeight(float height) 
{
    fHeightOverride = height;
}

EVec2 EUIField::GetSize() const
{
    return fCalculatedSize;
}

float EUIField::GetWidth() const
{
    return fCalculatedSize.x;
}

float EUIField::GetHeight() const
{
    return fCalculatedSize.y;
}

void EUIField::SetContextMenu(const ERef<EUIField>& menu) 
{
    fContextMenu = menu;   
}

void EUIField::SetTooltip(const ERef<EUIField>& tooltip)
{
    fToolTip = tooltip;
}

bool EUIField::IsContextMenuOpen() const
{
    return fIsContextMenuOpen;
}

bool EUIField::IsTooltipOpen() const
{
    return fIsTooltipOpen;
}

void EUIField::SetDirty() 
{
    fDirty = true;
}

void EUIField::SetDragType(const EString& type)
{
    fDragType = type;
}

void EUIField::SetDragData(EUIDragData data)
{
    fDragData = data;
}

void EUIField::AcceptDrag(const EString& type)
{
    fAcceptDragType = type;
}

EUIPanel::EUIPanel(const EString& title) 
    : EUIField(title), fOpen(true)
{
    
}

bool EUIPanel::OnRender() 
{
    if (fOpen)
    {
        ImGui::Begin(GetLabel().c_str(), &fOpen);
        ImGui::GetCurrentWindow()->Viewport->Flags &= ~ImGuiViewportFlags_NoDecoration;
        if (!fOpen) 
        {
            fWasJustClosed = true;
        }
    }
    return fOpen;
}

void EUIPanel::OnRenderEnd() 
{
    if (fOpen || fWasJustClosed)
    {
        fWasJustClosed = false;
        ImGui::End();
    }
}

bool EUIPanel::IsOpen() const
{
    return fOpen;
}

void EUIPanel::Close() 
{
    fOpen = false;
}

void EUIPanel::Open() 
{
    fOpen = true;
}

EUIViewport::EUIViewport(const Renderer::RCamera& camera) 
    :   EUIField("VIEWPORT"), 
        fFrameBuffer(Graphics::Wrapper::CreateFrameBuffer(100, 100)), 
        fRenderer(Graphics::Wrapper::GetMainContext(), fFrameBuffer),
        fCamera(camera)
{
    
}

EUIViewport::~EUIViewport() 
{
    if (fFrameBuffer)
    {
        delete fFrameBuffer;
    }
}

Renderer::RScene& EUIViewport::GetScene() 
{
    return fScene;
}

const Renderer::RScene& EUIViewport::GetScene() const
{
    return fScene;
}

const Renderer::RCamera& EUIViewport::GetCamera() const
{
    return fCamera;
}

Renderer::RCamera& EUIViewport::GetCamera() 
{
    return fCamera;
}

EUISameLine::EUISameLine() 
    : EUIField("OL")
{
    
}

bool EUISameLine::OnRender() 
{
    ImGui::SameLine();
    return false;
}

bool EUIViewport::OnRender() 
{
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

    fFrameBuffer->Resize(contentRegion.x, contentRegion.y, Graphics::GFrameBufferFormat::RGBA8);
    fRenderer.Render(&fScene, &fCamera);

    ImGui::Image((ImTextureID)(unsigned long long)(unsigned long)fFrameBuffer->GetColorAttachment(), contentRegion, {0, 1}, {1, 0});

    return true;
}


EUIButton::EUIButton(const EString& label) 
    : EUIField(label)
{
    
}

bool EUIButton::OnRender() 
{
    if (ImGui::Button(GetLabel().c_str(), {fWidthOverride, fHeightOverride}))
    {
        fEventDispatcher.Enqueue<events::EButtonEvent>(events::EButtonEvent());
    }
    return true;
}

EUILabel::EUILabel(const EString& label) 
    : EUIField(label)
{
    
}

bool EUILabel::OnRender() 
{
    ImGui::Text("%s", GetLabel().c_str());
    return true;
}

EUITextField::EUITextField(const EString& label, const EString& content)
    : EUIField(label), fContent(content)
{
    
}

bool EUITextField::OnRender() 
{
    char text[255];
    memcpy(text, fContent.c_str(), 255);

    if (ImGui::InputText(GetLabel().c_str(), text, 255))
    {
        fEventDispatcher.Enqueue<events::ETextChangeEvent>({fContent});
    }
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        fEventDispatcher.Enqueue<events::ETextCompleteEvent>({fContent});
    }
    fContent = text;
    return true;
}

void EUITextField::SetValue(const EString& value)
{
    fContent = value;
}

EString EUITextField::GetContent() const
{
    return fContent;
}

EUIFloatEdit::EUIFloatEdit(const EString& label) 
    : EUIField(label), fStep(0.1), fMin(0), fMax(100000), fValue(0)
{
    
}

bool EUIFloatEdit::OnRender() 
{
    if (ImGui::DragFloat(GetLabel().c_str(), &fValue, fStep, fMin, fMax, "%.3f", 1.0f))
    {
        fEventDispatcher.Enqueue<events::EFloatChangeEvent>({fValue});
    }
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        fEventDispatcher.Enqueue<events::EFloatCompleteEvent>({fValue});
    }

    return true;
}

void EUIFloatEdit::SetValue(float value)
{
    fValue = value;
}

float EUIFloatEdit::GetValue() const
{
    return fValue;
}


EUIIntegerEdit::EUIIntegerEdit(const EString& label) 
    : EUIField(label), fValue(0), fMin(-10000), fMax(10000)
{
    
}

bool EUIIntegerEdit::OnRender() 
{
    if (ImGui::DragInt(GetLabel().c_str(), &fValue, 1.0f, fMin, fMax))
    {
        fEventDispatcher.Enqueue<events::EIntegerChangeEvent>({fValue});
    }
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        fEventDispatcher.Enqueue<events::EIntegerCompleteEvent>({fValue});
    }
    return true;
}

void EUIIntegerEdit::SetValue(i32 value)
{
    fValue = value;
}

i32 EUIIntegerEdit::GetValue() const
{
    return fValue;
}


EUICheckbox::EUICheckbox(const EString& label) 
    : EUIField(label), fChecked(false)
{
    
}

bool EUICheckbox::OnRender() 
{
    if (ImGui::Checkbox(GetLabel().c_str(), &fChecked))
    {
        fEventDispatcher.Enqueue<events::ECheckboxEvent>({fChecked});
    }
    return true;
}

void EUICheckbox::SetValue(bool checked)
{
    fChecked = checked;
}

bool EUICheckbox::GetValue() const
{
    return fChecked;
}


// ----------------------------------------
// Main Menu Bar
EUIMainMenuBar::EUIMainMenuBar() 
    : EUIField("MainMenuBar")
{

}

bool EUIMainMenuBar::OnRender() 
{
    return fOpen = ImGui::BeginMainMenuBar();
}

void EUIMainMenuBar::OnRenderEnd() 
{
    if (fOpen)
    {
        ImGui::EndMainMenuBar();
    }
}


// ----------------------------------------
// Menu
EUIMenu::EUIMenu(const EString& displayName) 
    : EUIField(displayName), fOpen(false)
{
    
}

bool EUIMenu::OnRender() 
{
    return fOpen = ImGui::BeginMenu(GetLabel().c_str());
}

void EUIMenu::OnRenderEnd() 
{
    if (fOpen)
    {
        ImGui::EndMenu();
    }
    
}


// ----------------------------------------
// Context Menu
EUIContextMenu::EUIContextMenu(const EString& displayName) 
    : EUIField(displayName), fOpen(false)
{
    
}

bool EUIContextMenu::OnRender() 
{
    return fOpen = ImGui::BeginPopupContextWindow();
}

void EUIContextMenu::OnRenderEnd() 
{
    if (fOpen)
    {
        ImGui::EndPopup();
    }
}

// ----------------------------------------
// Menu Item
EUIMenuItem::EUIMenuItem(const EString& label) 
    : EUIField(label)
{
    
}

bool EUIMenuItem::OnRender() 
{
    if (ImGui::MenuItem(GetLabel().c_str()))
    {
        fEventDispatcher.Enqueue<events::EButtonEvent>(events::EButtonEvent());
    }
    return true;
}

EUIImageView::EUIImageView() 
    : EUIField("ImageView")
{
    fTexture = Graphics::Wrapper::CreateTexture();
    fTexture->SetTextureFormat(Graphics::GTextureFormat::RGBA8);
}

EUIImageView::~EUIImageView() 
{
    delete fTexture;
}

void EUIImageView::SetTextureData(u8* data, size_t width, size_t height) 
{
    fTexture->SetData(data, width, height);
}

bool EUIImageView::OnRender() 
{
    ImVec2 size = ImGui::GetContentRegionAvail();
    if (fWidthOverride != 0)
    {
        size.x = fWidthOverride;
    }
    if (fHeightOverride != 0)
    {
        size.y = fHeightOverride;
    }
    
    ImGui::Image((ImTextureID)(u64)fTexture->GetID(), size);
    return true;
}

EUIModal::EUIModal(const EString& title) 
    : EUIField(title)
{
    fOpen = false;
    fEndPopup = false;
    fPopupShouldOpen = false;
    SetWidth(300);
}

bool EUIModal::OnRender() 
{
    if (fPopupShouldOpen)
    {
        ImGui::OpenPopup(GetLabel().c_str());
        fPopupShouldOpen = false;
    }
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    fEndPopup = ImGui::BeginPopupModal(GetLabel().c_str(), &fOpen, ImGuiWindowFlags_AlwaysAutoResize);
    return fEndPopup;
}

void EUIModal::OnRenderEnd() 
{
    if (fEndPopup)
    {
        ImGui::EndPopup();
    }
}

void EUIModal::Open() 
{
    fOpen = true;
    fPopupShouldOpen = true;
}

void EUIModal::Close() 
{
    fOpen = false;
}

EUIContainer::EUIContainer(const EString& name) 
    : EUIField(name)
{
    
}

bool EUIContainer::OnRender() 
{
    ImGui::BeginChild(GetLabel().c_str(), {fWidthOverride, fHeightOverride});
    return true;
}

void EUIContainer::OnRenderEnd() 
{
    ImGui::EndChild();
}

EUISelectable::EUISelectable(const EString& label) 
    : EUIField(label), fStretchToAllColumns(false), fIsSelected(false)
{
    
}

bool EUISelectable::OnRender() 
{
    ImGuiSelectableFlags flags = fStretchToAllColumns ? ImGuiSelectableFlags_SpanAllColumns : 0;

    if (ImGui::Selectable(GetLabel().c_str(), &fIsSelected, flags))
    {
        fEventDispatcher.Enqueue<events::ESelectionChangeEvent>({fIsSelected});
    }
    return true;
}

void EUISelectable::SetStretchToAllColumns(bool stretch) 
{
    fStretchToAllColumns = stretch;
}

void EUISelectable::SetSelected(bool selected) 
{
    fIsSelected = selected;
}

bool EUISelectable::IsSelected() const
{
    return fIsSelected;
}

EUITable::EUITable(const EString& name) 
    : EUIField(name)
{
    
}

bool EUITable::OnRender() 
{
    if (fHeaderCells.size() == 0) { return fEndTable = false; }
    static ImGuiTableFlags flags = ImGuiTableFlags_Resizable;
    //ImGui::CheckboxFlags("ImGuiTableFlags_Resizable", &flags, ImGuiTableFlags_Resizable);
    //ImGui::CheckboxFlags("ImGuiTableFlags_Reorderable", &flags, ImGuiTableFlags_Reorderable);
    //ImGui::CheckboxFlags("ImGuiTableFlags_Hideable", &flags, ImGuiTableFlags_Hideable);
    //ImGui::CheckboxFlags("ImGuiTableFlags_NoBordersInBody", &flags, ImGuiTableFlags_NoBordersInBody);
    //ImGui::CheckboxFlags("ImGuiTableFlags_NoBordersInBodyUntilResize", &flags, ImGuiTableFlags_NoBordersInBodyUntilResize);

    if ((fEndTable = ImGui::BeginTable(GetLabel().c_str(), fHeaderCells.size(), flags)))
    {
        for (const EString& header : fHeaderCells)
        {
            ImGui::TableSetupColumn(header.c_str());
        }
        ImGui::TableHeadersRow();
    }
    return fEndTable;
}

void EUITable::OnRenderEnd() 
{
    if (fEndTable)
    {
        ImGui::EndTable();
    }
}

void EUITable::SetHeaderCells(const EVector<EString>& headerCells) 
{
    fHeaderCells = headerCells;
}

EUITableRow::EUITableRow()
    : EUIField("TR"), fCurrentTableIndex(0)
{
    
}

void EUITableRow::OnBeforeChildRender() 
{
    ImGui::TableSetColumnIndex(fCurrentTableIndex++);
}

bool EUITableRow::OnRender() 
{
    fCurrentTableIndex = 0;
    ImGui::TableNextRow();
    return true;
}

EUIGrid::EUIGrid(float size)
    : EUIGrid(size, size)
{

}

EUIGrid::EUIGrid(float cellWidth, float cellHeight)
    : EUIField("GRID"), fCellWidth(cellWidth), fCellHeight(cellHeight), fCurrentChildCount(0)
{

}

void EUIGrid::SetCellSize(float size)
{
    SetCellWidth(size);
    SetCellHeight(size);
}

void EUIGrid::SetCellWidth(float width)
{
    fCellWidth = width;
}

void EUIGrid::SetCellHeight(float height)
{
    fCellHeight = height;
}


bool EUIGrid::OnRender()
{
    fCurrentChildCount = 0;
    
    fCurrentWidthAvail = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
    return true;
}

void EUIGrid::OnBeforeChildRender()
{
    ImGuiStyle& style = ImGui::GetStyle();
    if (fCurrentChildCount != 0)
    {
        float last_button_x2 = ImGui::GetItemRectMax().x;
        float next_button_x2 = last_button_x2 + style.ItemSpacing.x + fCellWidth; // Expected position if next button was on same line
        if (fCurrentChildCount + 1 < fChildren.size() && next_button_x2 < fCurrentWidthAvail)
        {
            ImGui::SameLine();
        }
    }
    fCurrentChildCount++;
    ImGui::SetNextItemWidth(fCellWidth);
}

void EUIGrid::OnAfterChildRender()
{
}