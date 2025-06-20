#pragma once

TOSHI_NAMESPACE_START

constexpr const TCHAR* TDebugConsoleUUID = "{1B5D8052-529A-4c88-9DDE-EE523D440B00}";

class TDebugConsole
{
public:
	TDebugConsole();
	~TDebugConsole();

	TBOOL Show(TBOOL state);
	TBOOL Toggle();

	// Returns TTRUE if the console is visible
	TBOOL IsVisible() const { return m_Visible; }

	// Returns TTRUE if the console is created
	TBOOL IsCreated() const { return m_Created; }

	// Tries to find handle of the debug console
	HWND FindConsole() const;

private:
	TBOOL m_Visible = TFALSE;
	TBOOL m_Created = TFALSE;
	HWND  m_HWND;
};

TOSHI_NAMESPACE_END
