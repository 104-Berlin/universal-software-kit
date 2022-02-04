#include "editor.h"

using namespace Editor;
using namespace Engine;

EBasicRegisterView::EBasicRegisterView()
    : EUIField("BasicRegisterView")
{
    ERef<EUITabView> tabView = EMakeRef<EUITabView>();
    tabView->AddTab("Tasks", CreateTaskView());
    tabView->AddTab("Resources", CreateResourceView());

    AddChild(tabView);
}

ERef<EUIField> EBasicRegisterView::CreateTaskView() 
{
    ERef<EUIField> taskView = EMakeRef<EUIField>("TaskView");
    EWeakRef<EUIField> weakTaskView = taskView;
    taskView->SetDirty();
    taskView->SetCustomUpdateFunction([weakTaskView](){
        if (weakTaskView.expired()) { return; }

        weakTaskView.lock()->Clear();
        EVector<EBaseTask*> tasks = shared::ExtensionManager().GetTaskRegister().GetAllItems();
        for (EBaseTask* task : tasks)
        {
            ERef<EUIField> taskField = EMakeRef<EUIField>(task->GetName() + "_task");
            taskField->AddChild(EMakeRef<EUILabel>(task->GetName()));
            taskField->AddChild(EMakeRef<EUISameLine>());
            ERef<EUIButton> runButton = EMakeRef<EUIButton>("Run");
            EWeakRef<EUIButton> weakRunButton = runButton;
            runButton->AddEventListener<events::EButtonEvent>([task, weakRunButton](){
                const EValueDescription& intputDescription = task->GetInputDescription();
                ERef<EProperty> input = nullptr;
                if (task->HasInput() && intputDescription.Valid())
                {
                    if (intputDescription.GetType() != EValueType::STRUCT)
                    {
                        E_ERROR("Task input is not a struct");
                    }
                    else
                    {
                        input = EProperty::CreateFromDescription("IN", intputDescription);
                        ERef<EUIField> editPopup = EMakeRef<EUIField>("EditPopup");
                        editPopup->AddChild(EMakeRef<EComponentEdit>(ERef<EProperty>(input)));
                        editPopup->AddChild(EMakeRef<EUIButton>("OK")).lock()->AddEventListener<events::EButtonEvent>([task, input](){
                            task->Execute(input);
                        });
                        weakRunButton.lock()->OpenPopup(editPopup);
                    }
                }
                else
                {
                    task->Execute(input);
                }
            });
            taskField->AddChild(runButton);
            weakTaskView.lock()->AddChild(taskField);
        }
    });
    return taskView;
} 

ERef<EUIField> EBasicRegisterView::CreateResourceView() 
{
    ERef<EUIField> resourceView = EMakeRef<EUIField>("ResourceView");
    resourceView->AddChild(EMakeRef<EUILabel>("ResourceView"));
    return resourceView;
}  