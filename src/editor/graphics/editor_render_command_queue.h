#pragma once

#define DEFINE_VAR_1(code, ...)
#define DEFINE_VAR_2(var, ...)    typename std::remove_const<typename ::std::remove_reference<decltype(var)>::type>::type var;\
									EXPAND(DEFINE_VAR_1(__VA_ARGS__))
#define DEFINE_VAR_3(var, ...)    typename std::remove_const<typename ::std::remove_reference<decltype(var)>::type>::type var;\
									EXPAND(DEFINE_VAR_2(__VA_ARGS__))
#define DEFINE_VAR_4(var, ...)    typename std::remove_const<typename ::std::remove_reference<decltype(var)>::type>::type var;\
                                    EXPAND(DEFINE_VAR_3(__VA_ARGS__))
#define DEFINE_VAR_5(var, ...)    typename std::remove_const<typename ::std::remove_reference<decltype(var)>::type>::type var;\
                                    EXPAND(DEFINE_VAR_4(__VA_ARGS__))


#define DEFINE_VAR_N(N, ...) EXPAND(CONCATENATE(DEFINE_VAR_, N) (__VA_ARGS__))



#define EXEC_FUNCTION_1(code) code
#define EXEC_FUNCTION_2(var, code) auto& var = this->var;\
									EXPAND(EXEC_FUNCTION_1(code))

#define EXEC_FUNCTION_3(var, ...) auto& var = this->var;\
									EXPAND(EXEC_FUNCTION_2(__VA_ARGS__))

#define EXEC_FUNCTION_4(var, ...) auto& var = this->var;\
									EXPAND(EXEC_FUNCTION_3(__VA_ARGS__))
#define EXEC_FUNCTION_5(var, ...) auto& var = this->var;\
                                    EXPAND(EXEC_FUNCTION_4(__VA_ARGS__))
#define EXEC_FUNCTION_6(var, ...) auto& var = this->var;\
                                    EXPAND(EXEC_FUNCTION_5(__VA_ARGS__))



#define EXEC_FUNCTION_N(N, ...) EXPAND(CONCATENATE(EXEC_FUNCTION_, N) (__VA_ARGS__))


#define EXEC_CONSTRUCTOR_ARGS_1(code) 
#define EXEC_CONSTRUCTOR_ARGS_2(var, code) typename ::std::remove_const<typename ::std::remove_reference<decltype(var)>::type>::type var\
											EXPAND(EXEC_CONSTRUCTOR_ARGS_1(code))

#define EXEC_CONSTRUCTOR_ARGS_3(var, ...) typename ::std::remove_const<typename ::std::remove_reference<decltype(var)>::type>::type var,\
											EXPAND(EXEC_CONSTRUCTOR_ARGS_2(__VA_ARGS__))

#define EXEC_CONSTRUCTOR_ARGS_4(var, ...) typename ::std::remove_const<typename ::std::remove_reference<decltype(var)>::type>::type var,\
											EXPAND(EXEC_CONSTRUCTOR_ARGS_3(__VA_ARGS__))
#define EXEC_CONSTRUCTOR_ARGS_5(var, ...) typename ::std::remove_const<typename ::std::remove_reference<decltype(var)>::type>::type var,\
                                            EXPAND(EXEC_CONSTRUCTOR_ARGS_4(__VA_ARGS__))
#define EXEC_CONSTRUCTOR_ARGS_6(var, ...) typename ::std::remove_const<typename ::std::remove_reference<decltype(var)>::type>::type var,\
                                            EXPAND(EXEC_CONSTRUCTOR_ARGS_5(__VA_ARGS__))



#define EXEC_CONSTRUCTOR_ARGS_N(N, ...) EXPAND(CONCATENATE(EXEC_CONSTRUCTOR_ARGS_, N) (__VA_ARGS__))


#define EXEC_CONSTRUCTOR_INIT_1(code) 
#define EXEC_CONSTRUCTOR_INIT_2(var, code) this->var = var;\
											EXPAND(EXEC_CONSTRUCTOR_INIT_1(code))

#define EXEC_CONSTRUCTOR_INIT_3(var, ...) this->var = var;\
											EXPAND(EXEC_CONSTRUCTOR_INIT_2(__VA_ARGS__))

#define EXEC_CONSTRUCTOR_INIT_4(var, ...) this->var = var;\
											EXPAND(EXEC_CONSTRUCTOR_INIT_3(__VA_ARGS__))
#define EXEC_CONSTRUCTOR_INIT_5(var, ...) this->var = var;\
                                            EXPAND(EXEC_CONSTRUCTOR_INIT_4(__VA_ARGS__))
#define EXEC_CONSTRUCTOR_INIT_6(var, ...) this->var = var;\
                                            EXPAND(EXEC_CONSTRUCTOR_INIT_5(__VA_ARGS__))



#define EXEC_CONSTRUCTOR_INIT_N(N, ...) EXPAND(CONCATENATE(EXEC_CONSTRUCTOR_INIT_, N) (__VA_ARGS__))


#define E_RENDERCOMMAND(...) struct CONCATENATE(_RenderCommand_, __LINE__) : public ::Engine::ERenderCommand{\
							EXPAND(DEFINE_VAR_N(EXPAND(ARG_COUNT(__VA_ARGS__)), __VA_ARGS__))\
                            CONCATENATE(_RenderCommand_, __LINE__) ( EXPAND(EXEC_CONSTRUCTOR_ARGS_N(EXPAND(ARG_COUNT(__VA_ARGS__)), __VA_ARGS__)) ) {\
                                EXPAND(EXEC_CONSTRUCTOR_INIT_N(EXPAND(ARG_COUNT(__VA_ARGS__)), __VA_ARGS__))\
                            }\
                            virtual void Execute() {\
	                            EXPAND(EXEC_FUNCTION_N(EXPAND(ARG_COUNT(__VA_ARGS__)), __VA_ARGS__))\
                            }\
                        };
namespace Editor {

	class ERenderCommand
	{
	public:
		virtual ~ERenderCommand() = default;

		virtual void Execute() = 0;
	};

	class ERenderCommandQueue
	{
	public:
		ERenderCommandQueue();
		~ERenderCommandQueue();

		void AddRenderCommand(ERenderCommand* renderCommand);

		void Execute();
	private:
		EQueue<ERenderCommand*> fCommandQueue;
	};

}
