#ifndef MODULE_H_
#define MODULE_H_

template <class T>
class Module {
public:
	Module(T sampleRate);
	virtual void reset() = 0;
	virtual void io(AudioBuffer<T> in, AudioBuffer<T> out);

private:
	AudioBuffer<T> m_in;
	AudioBuffer<T> m_out;
	T m_fs;


};

#endif