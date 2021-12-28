#pragma once
#include"ListWindowController.h"

ListWindowController::ListWindowController(HINSTANCE hInstance, int window_width, int window_height) : BaseWindowController(hInstance, window_width, window_height) {

}

ListWindowController::~ListWindowController() {

}

HRESULT ListWindowController::CreateChildWindow() {
	//new�{�^���傫��
	int btn_width = m_client_width / (m_btnCnt + 5);
	int btn_height = m_client_height / 40;
	//�{�^����
	const char* btn_name[m_btnCnt] =
	{
		"new",
		"delete",
		"edit",
	};
	//�{�^���ʒu
	int btn_offsetX = 0;
	int btn_offsetY[m_btnCnt] = { 0,btn_height * 2,btn_height * 3 };

	//�R���{�{�b�N�X�傫��
	int cb_width = m_client_width - btn_width;
	int cb_height = m_client_height;
	//�R���{�{�b�N�X�ʒu
	int cb_offsetX = btn_width;
	int cb_offsetY = 0;


	//���X�g�{�b�N�X�傫��
	int lb_width = (m_client_width - btn_width) / 2 - 30;
	int lb_height = m_client_height - btn_height * 2 - 15;
	//���X�g�{�b�N�X�ʒu
	int lb_offsetX[m_lbCnt] = { btn_width + 15, lb_offsetX[0] + lb_width + 15};
	int lb_offsetY[m_lbCnt] = { btn_height * 2, btn_height * 2 };

	if (FAILED(CreateButton(m_bhwnd[0], btn_name[0], btn_offsetX, btn_offsetY[0], btn_width, btn_height, m_newbtnID))) {
		return ErrorMessage("Failed to Create ChildWindow");
	}
	if (FAILED(CreateButton(m_bhwnd[1], btn_name[1], btn_offsetX, btn_offsetY[1], btn_width, btn_height, m_deletebtnID))) {
		return ErrorMessage("Failed to Create ChildWindow");
	}
	if (FAILED(CreateButton(m_bhwnd[2], btn_name[2], btn_offsetX, btn_offsetY[2], btn_width, btn_height, m_editbtnID))) {
		return ErrorMessage("Failed to Create ChildWindow");
	}
	if (FAILED(CreateDropDownComboBox(m_chwnd, "ComboBox", cb_offsetX, cb_offsetY, cb_width, cb_height,m_cbID))) {
		return ErrorMessage("Failed to Create ChildWindow");
	}
	if(FAILED(CreateListBox(m_lhwnd[0], "ListBox", lb_offsetX[0], lb_offsetY[0], lb_width, lb_height, m_lbParentID))){
		return ErrorMessage("Failed to Create ChildWindow");
	}
	if (FAILED(CreateListBox(m_lhwnd[1], "ListBox", lb_offsetX[1], lb_offsetY[1], lb_width, lb_height, m_lbChildID))) {
		return ErrorMessage("Failed to Create ChildWindow");
	}
	return S_OK;
}

HRESULT ListWindowController::InitChildWindow() {
	//�R���{�{�b�N�X�̒��ɓǂݍ��݉\�ȃ��f���t�@�C����񋓂���
	PathController pc;
	//�T���p������
	char objsPath[MAX_PATH_LENGTH];
	pc.AddLeafPath(pc.basePath, objsPath, "\\ObjectViewer_D3D12\\Model\\OBJ\\*.obj");

	HANDLE hFind;
	WIN32_FIND_DATA fd;
	//������Ȃ��Ȃ�܂ŁA�Y���f�B���N�g�����ɂĊg���q��.obj�̃t�@�C����T��
	hFind = FindFirstFile(objsPath, &fd);
	if (hFind == INVALID_HANDLE_VALUE) {

	}
	else {
		char objFilesPath[MAX_PATH_LENGTH];
		pc.AddLeafPath("\\ObjectViewer_D3D12\\Model\\OBJ\\", objFilesPath, fd.cFileName);
		m_loadableFileList.push_back(objFilesPath);
		while (FindNextFile(hFind, &fd)) {
			pc.AddLeafPath("\\ObjectViewer_D3D12\\Model\\OBJ\\", objFilesPath, fd.cFileName);
			m_loadableFileList.push_back(objFilesPath);
		}
	}

	//�擾�����t�@�C�����X�g���R���{�{�b�N�X�ɗ񋓂���
	for (int i = 0; i < m_loadableFileList.size(); i++) {
		char name[MAX_PATH_LENGTH];
		pc.GetLeafDirectryName(m_loadableFileList[i].c_str(), name, MAX_PATH_LENGTH);
		SendMessage(m_chwnd, CB_ADDSTRING, 0, (LPARAM)name);
	}

	return S_OK;
}

LRESULT ListWindowController::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) { //���b�Z�[�W���e�𒲂ׂ�
	case WM_COMMAND:
		switch (LOWORD(wParam)) { //ID�𒲂ׂ�
		case m_newbtnID : //"new"�{�^���̎�
			if (!m_isLoad) {
				m_isLoad = true;
			}
			return 0;
		case m_deletebtnID : //"delete"�{�^���̎�
			if (!m_isDelete) {
				m_isDelete = true;
			}
			return 0;
		case m_cbID : //"ComboBox"�̎�
			switch (HIWORD(wParam)) { //���b�Z�[�W���e�𒲂ׂ�
			case CBN_SELCHANGE :
				m_loadIdx = SendMessage(m_chwnd, CB_GETCURSEL, 0, 0); //�I�����ڂ̃C���f�b�N�X���擾
				return 0;
			}
			return 0;
		case m_lbParentID : //"ParentListBox"�̎�
			switch (HIWORD(wParam)) { //���b�Z�[�W���e�𒲂ׂ�
			case LBN_SELCHANGE :
				int p = m_parentIdx;
				m_parentIdx = SendMessage(m_lhwnd[0], LB_GETCURSEL, 0, 0);
				m_childIdx = 0;
				if (p != m_parentIdx) {
					m_isParentChanged = true;
					m_isChildChanged = true;
				}
				return 0;
			}
			return 0;
		case m_lbChildID: //"ChildListBox"�̎�
			switch (HIWORD(wParam)) { //���b�Z�[�W���e�𒲂ׂ�
			case LBN_SELCHANGE :
				int p = m_childIdx;
				m_childIdx = SendMessage(m_lhwnd[1], LB_GETCURSEL, 0, 0);
				if (p != m_childIdx) {
					m_isChildChanged = true;
				}
				return 0;
			}
			return 0;
		}

		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}