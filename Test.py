from ctypes import *

usk = cdll.LoadLibrary("./bin/libEInterface.dylib")

usk.CreateComponent_c_str("TestComponent".encode("utf-8"), 1)