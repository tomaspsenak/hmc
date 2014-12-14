#pragma once

/*
     Copyright (C) 2014 Tomáš Pšenák
     This program is free software; you can redistribute it and/or modify 
     it under the terms of the GNU General Public License version 2 as 
     published by the Free Software Foundation.
 
     This program is distributed in the hope that it will be useful, but 
     WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
     or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
     for more details.
 
     You should have received a copy of the GNU General Public License along 
     with this program; if not, write to the Free Software Foundation, Inc., 
     51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#if !defined(DESKTOPSOURCESILENCEGENERATOR_HMCENCODER_INCLUDED)
#define DESKTOPSOURCESILENCEGENERATOR_HMCENCODER_INCLUDED

#include <mmdeviceapi.h>

class DesktopSourceSilenceGenerator
{
	public:		DesktopSourceSilenceGenerator(const DesktopSourceSilenceGenerator & obj);
				DesktopSourceSilenceGenerator & operator = (const DesktopSourceSilenceGenerator & obj);
				~DesktopSourceSilenceGenerator(void);

				HRESULT Start(IMMDevice * device);
				void Stop(void);
				HRESULT GeneratorHR(void)
				{
					return this->m_hr;
				}

				static void Init(void);
				static DesktopSourceSilenceGenerator * Get(void)
				{
					return g_singleton;
				}

	private:	DesktopSourceSilenceGenerator(void);

				CCritSec m_mainLock;
				IMMDevice * m_device;
				HANDLE m_startedEvent;
				HANDLE m_stopEvent;
				HANDLE m_thread;
				HRESULT m_hr;
				DWORD m_count;

				static DesktopSourceSilenceGenerator * g_singleton;
				static DWORD WINAPI PlaySilenceFunction(LPVOID pContext);
};

#endif //DESKTOPSOURCESILENCEGENERATOR_HMCENCODER_INCLUDED

