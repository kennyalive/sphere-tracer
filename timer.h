#ifndef _TIMER_H_
#define _TIMER_H_

//---------------------------------------------------------------------
class Timer {
public:
	Timer() {
		m_prevTime = GetTime();
		m_frameTime = 0.f;
		m_lastFPSUpdate = m_prevTime;
		m_fps = -1.f;
		m_FPSValues.reserve(128);
	}
	void Update() {
		const double time = GetTime();
		m_frameTime = time - m_prevTime;
		m_prevTime = time;

		float fps = float(1.0 / m_frameTime);
		m_FPSValues.push_back(fps);
		
		if (time - m_lastFPSUpdate > 0.2f) {
			m_fps = 0.0f;
			for (size_t i = 0; i < m_FPSValues.size(); i++) {
				m_fps += m_FPSValues[i];
			}
			m_fps /= m_FPSValues.size();
			m_FPSValues.resize(0);
			m_lastFPSUpdate = time;
		}
	}

	bool GetFPS(float& fps) {
		if (m_fps == -1.f) {
			return false;
		}
		fps = m_fps;
		m_fps = -1;
		return true;
	}
	double GetFrameTime() const {
		return m_frameTime;
	}

private:
	static double GetTime() {
		__int64 count, freq;
		QueryPerformanceCounter((LARGE_INTEGER*)&count);
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		return double(count) / double(freq);
	}

private:
	double m_prevTime;
	double m_frameTime;
	double m_lastFPSUpdate;
	float m_fps;
	std::vector<float> m_FPSValues;
};

#endif //_TIMER_H_
