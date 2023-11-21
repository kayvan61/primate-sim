from dataclasses import dataclass

@dataclass
class Global_Params:
    debug: bool = False

GLOBAL_PARAMS = Global_Params()