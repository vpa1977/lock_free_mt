
Создание окна:

WM_ACTIVATE active
WM_SIZE RESTORED
WM_MOVE


Перемещение окна:

WM_ENTERSIZEMOVE
(WM_MOVING,WM_MOVE)...
WM_EXITSIZEMOVE


Изменение размера окна:

WM_ENTERSIZEMOVE
(WM_SIZING,WM_SIZE)...
WM_EXITSIZEMOVE


Maximize:

WM_MOVE x=0
WM_SIZE MAXIMIZED


Minimize:

WM_MOVE x<0 y<0
WM_SIZE MINIMIZED w=0 h=0
WM_ACTIVATE inactive minimized


Restore из minimized:

WM_ACTIVATE active minimized
WM_MOVE
WM_SIZE RESTORED
WM_ACTIVATE active


Потеря фокуса (например из-за alttab-а):

WM_ACTIVATE inactive


Получение фокуса:

WM_ACTIVATE active
или
WM_ACTIVATE active_by_click

