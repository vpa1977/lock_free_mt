
�������� ����:

WM_ACTIVATE active
WM_SIZE RESTORED
WM_MOVE


����������� ����:

WM_ENTERSIZEMOVE
(WM_MOVING,WM_MOVE)...
WM_EXITSIZEMOVE


��������� ������� ����:

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


Restore �� minimized:

WM_ACTIVATE active minimized
WM_MOVE
WM_SIZE RESTORED
WM_ACTIVATE active


������ ������ (�������� ��-�� alttab-�):

WM_ACTIVATE inactive


��������� ������:

WM_ACTIVATE active
���
WM_ACTIVATE active_by_click

