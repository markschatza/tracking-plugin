/*
  ==============================================================================

    TrackingStimulator.cpp
    Created: 11 Mar 2016 11:59:38am
    Author:  alessio

  ==============================================================================
*/

#include "TrackingStimulator.h"
#include "TrackingStimulatorEditor.h"

#include "../../UI/EditorViewport.h"

TrackingStimulator::TrackingStimulator()
    : GenericProcessor("Tracking Stim")
    , m_isOn(false)
    , m_x(-1.0)
    , m_y(-1.0)
    , m_height(1.0)
    , m_width(1.0)
    , m_positionIsUpdated(false)
    , m_positionDisplayedIsUpdated(false)
    , m_simulateTrajectory(false)
    , m_selectedCircle(-1)
    , m_tot_chan(4)
    , m_timePassed(0.0)
    , m_currentTime(0.0)
    , m_previousTime(0.0)
    , m_timePassed_sim(0.0)
    , m_currentTime_sim(0.0)
    , m_previousTime_sim(0.0)
    , m_count(0)
    , m_forward(true)
    , m_rad(0.0)
    , m_chan(0)
    , m_pulsePal()
    , m_TTLSyncChan(0)
    , m_StimSyncChan(4)
{

    setProcessorType (PROCESSOR_TYPE_SINK);

    // Init PulsePal
    m_pulsePal.initialize();
    m_pulsePal.setDefaultParameters();
    m_pulsePal.updateDisplay("GUI Connected","Click for menu");
    m_pulsePalVersion = m_pulsePal.getFirmwareVersion();

    m_circles = vector<Circle>();

    m_stimFreq = vector<float>(m_tot_chan, DEF_FREQ);
    m_stimSD = vector<float>(m_tot_chan, DEF_SD);

    m_phaseDuration = vector<float>(m_tot_chan, DEF_PHASE_DURATION);
    m_interPhaseInt = vector<float>(m_tot_chan, DEF_INTER_PHASE);
    m_repetitions = vector<int>(m_tot_chan, DEF_REPETITIONS);
    m_voltage = vector<float>(m_tot_chan, DEF_VOLTAGE);
    m_interPulseInt = vector<float>(m_tot_chan, DEF_INTER_PULSE);
    m_trainDuration = vector<float>(m_tot_chan, DEF_TRAINDURATION);

    m_isUniform = vector<int>(m_tot_chan, 1);
    m_isBiphasic = vector<int>(m_tot_chan, 1);
    m_negativeFirst = vector<int>(m_tot_chan, 1);

}

TrackingStimulator::~TrackingStimulator()
{

}

AudioProcessorEditor* TrackingStimulator::createEditor()
{
    editor = new TrackingStimulatorEditor(this, true);
    return editor;
}


// Setters - Getters

float TrackingStimulator::getX() const
{
    return m_x;
}
float TrackingStimulator::getY() const
{
    return m_y;
}
float TrackingStimulator::getWidth() const
{
    return m_width;
}
float TrackingStimulator::getHeight() const
{
    return m_height;
}

bool TrackingStimulator::getSimulateTrajectory() const
{
    return m_simulateTrajectory;
}

void TrackingStimulator::setSimulateTrajectory(bool sim)
{
    m_simulateTrajectory = sim;
}

vector<Circle> TrackingStimulator::getCircles()
{
    return m_circles;
}
void TrackingStimulator::addCircle(Circle c)
{
    m_circles.push_back(c);
}
void TrackingStimulator::editCircle(int ind, float x, float y, float rad, bool on)
{
    m_circles[ind].set(x,y,rad,on);
}
void TrackingStimulator::deleteCircle(int ind)
{
    m_circles.erase(m_circles.begin() + ind);
}
void TrackingStimulator::disableCircles()
{
    for(int i=0; i<m_circles.size(); i++)
        m_circles[i].off();
}

int TrackingStimulator::getSelectedCircle() const
{
    return m_selectedCircle;
}
void TrackingStimulator::setSelectedCircle(int ind)
{
    m_selectedCircle = ind;
}

int TrackingStimulator::getChan() const
{
    return m_chan;
}


float TrackingStimulator::getStimFreq(int chan) const
{
    return m_stimFreq[chan];
}
float TrackingStimulator::getStimSD(int chan) const
{
    return m_stimSD[chan];
}
bool TrackingStimulator::getIsUniform(int chan) const
{
    if (m_isUniform[chan])
        return true;
    else
        return false;
}
bool TrackingStimulator::getIsBiphasic(int chan) const
{
    if (m_isBiphasic[chan])
        return true;
    else
        return false;
}
bool TrackingStimulator::getNegFirst(int chan) const
{
    if (m_negativeFirst[chan])
        return true;
    else
        return false;
}
float TrackingStimulator::getPhaseDuration(int chan) const
{
    return m_phaseDuration[chan];
}
float TrackingStimulator::getInterPhaseInt(int chan) const
{
    return m_interPhaseInt[chan];
}
float TrackingStimulator::getVoltage(int chan) const
{
    return m_voltage[chan];
}
int TrackingStimulator::getRepetitions(int chan) const
{
    return m_repetitions[chan];
}
float TrackingStimulator::getInterPulseInt(int chan) const
{
    return m_interPulseInt[chan];
}
float TrackingStimulator::getTrainDuration(int chan) const
{
    return m_trainDuration[chan];
}

uint32_t TrackingStimulator::getPulsePalVersion() const
{
    return m_pulsePalVersion;
}

void TrackingStimulator::setStimFreq(int chan, float stimFreq)
{
    m_stimFreq[chan] = stimFreq;
}
void TrackingStimulator::setStimSD(int chan, float stimSD)
{
    m_stimSD[chan] = stimSD;
}
void TrackingStimulator::setIsUniform(int chan, bool isUniform)
{
    if (isUniform)
        m_isUniform[chan] = 1;
    else
        m_isUniform[chan] = 0;
}
void TrackingStimulator::setIsBiphasic(int chan, bool isBiphasic)
{
    if (isBiphasic)
        m_isBiphasic[chan] = 1;
    else
        m_isBiphasic[chan] = 0;
}
void TrackingStimulator::setNegFirst(int chan, bool negFirst)
{
    if (negFirst)
        m_negativeFirst[chan] = 1;
    else
        m_negativeFirst[chan] = 0;

}
void TrackingStimulator::setPhaseDuration(int chan, float phaseDuration)
{
    m_phaseDuration[chan] = phaseDuration;
    //    updatePulsePal();
}
void TrackingStimulator::setInterPhaseInt(int chan, float interPhaseInt)
{
    m_interPhaseInt[chan] = interPhaseInt;
    //    updatePulsePal();
}
void TrackingStimulator::setVoltage(int chan, float voltage)
{
    m_voltage[chan] = voltage;
    //    updatePulsePal();
}
void TrackingStimulator::setRepetitions(int chan, int rep)
{
    m_repetitions[chan] = rep;
    //    updatePulsePal();
}
void TrackingStimulator::setInterPulseInt(int chan, float interPulseInt)
{
    m_interPulseInt[chan] = interPulseInt;
    //    updatePulsePal();
}
void TrackingStimulator::setTrainDuration(int chan, float trainDuration)
{
    m_trainDuration[chan] = trainDuration;
    //    updatePulsePal();
}

void TrackingStimulator::setChan(int chan)
{
    m_chan = chan;
}

void TrackingStimulator::setStimSyncChan(int chan)
{
    m_StimSyncChan = chan;
}

void TrackingStimulator::setTTLSyncChan(int chan)
{
    m_TTLSyncChan = chan;
}

bool TrackingStimulator::checkParameterConsistency(int chan)
{
    if (m_repetitions[chan] > 1)
        if (!m_isBiphasic[chan])
            return !(m_phaseDuration[chan] + m_interPulseInt[chan] > m_trainDuration[chan]);
        else
            return !(2*m_phaseDuration[chan] + m_interPhaseInt[chan] + m_interPulseInt[chan] > m_trainDuration[chan]);
    else
    {
        if (!m_isBiphasic[chan])
        {
            //            m_interPulseInt[chan] = m_phaseDuration[chan];
            m_trainDuration[chan] = m_phaseDuration[chan];
        }
        else
        {
            //            m_interPulseInt[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
            m_trainDuration[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
        }
        return true;
    }
}

void TrackingStimulator::setRepetitionsTrainDuration(int chan, priority whatFirst)
{
    if (whatFirst == REPFIRST)
    {
        if (m_repetitions[chan] > 1)
            if (!m_isBiphasic[chan])
                m_trainDuration[chan] = (m_phaseDuration[chan] + m_interPulseInt[chan])*m_repetitions[chan]; // + m_phaseDuration[chan];
            else
                m_trainDuration[chan] = (2*m_phaseDuration[chan] + m_interPhaseInt[chan] + m_interPulseInt[chan])*m_repetitions[chan]; // + (2*m_phaseDuration[chan]+ m_interPhaseInt[chan]);
        else
            if (!m_isBiphasic[chan])
                m_trainDuration[chan] = m_phaseDuration[chan];
            else
                m_trainDuration[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
    }
    else
    {
        if (!m_isBiphasic[chan])
            if (int(m_trainDuration[chan]/(m_phaseDuration[chan] + m_interPulseInt[chan])) > 1)
                m_repetitions[chan] = int(m_trainDuration[chan]/(m_phaseDuration[chan] + m_interPulseInt[chan]));
            else
            {
                m_repetitions[chan] = 1;
                m_trainDuration[chan] = m_phaseDuration[chan];
            }
        else
            if (int(m_trainDuration[chan]/(2*m_phaseDuration[chan] + m_interPulseInt[chan] + m_interPulseInt[chan]) > 1))
                m_repetitions[chan] = int(m_trainDuration[chan]/(2*m_phaseDuration[chan] + m_interPulseInt[chan] + m_interPulseInt[chan]));
            else
            {
                m_repetitions[chan] = 1;
                m_trainDuration[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
            }
    }
}

void TrackingStimulator::process(AudioSampleBuffer&)
{
    if (!m_simulateTrajectory)
    {
        checkForEvents();
    }
    else
    {
        // simulate events at 60Hz
        m_currentTime_sim = Time::currentTimeMillis();
        m_timePassed_sim = float(m_currentTime_sim - m_previousTime_sim)/1000.0; // in seconds

        if (m_timePassed_sim >= float(1.0/TRACKING_FREQ))
        {
            // generate new position sample
            m_positionIsUpdated = true;
            float theta = float(m_count/20.);

            if (m_forward)
                if (m_rad < 0.5)
                    m_rad = m_rad + 0.002;
                else
                    m_forward = false;
            else
                if (m_rad > 0)
                    m_rad = m_rad - 0.002;
                else
                    m_forward = true;


            m_x = m_rad*std::cos(theta) + 0.5;
            m_y = m_rad*std::sin(theta) + 0.5;

            //            std::cout << m_x << ', ' << m_y << endl;



            m_previousTime_sim = Time::currentTimeMillis();
            m_timePassed_sim = 0;
            m_count++;
        }

    }

    if (m_isOn){

        m_currentTime = Time::currentTimeMillis();
        m_timePassed = float(m_currentTime - m_previousTime)/1000.0; // in seconds

        lock.enter();

        // Check if current position is within stimulation areas
        bool stim = stimulate();

        if (stim)
        {
            // Check if timePassed >= latency

            float stim_interval;
            if (m_isUniform[m_chan]) //uniform
                stim_interval = float(1/m_stimFreq[m_chan]);
            else                     //gaussian
            {
                int circleIn = isPositionWithinCircles(m_x, m_y);
                float dist_norm = m_circles[circleIn].distanceFromCenter(m_x, m_y) / m_circles[circleIn].getRad();
                std::cout << "Norm dist: "  << dist_norm << endl;

                float k = -1.0 / std::log(m_stimSD[m_chan]);

                float freq_gauss = m_stimFreq[m_chan]*std::exp(-pow(dist_norm,2)/k);
                stim_interval = float(1/freq_gauss);
                std::cout << "Gauss_freq:" << endl;
                std::cout << freq_gauss << endl;
                std::cout << "Stim_interval:" << endl;
                std::cout << stim_interval << endl;
            }


            if (m_timePassed >= stim_interval)
            {
                // trigger selected channel
                m_pulsePal.triggerChannel(m_chan + 1);
                m_previousTime = Time::currentTimeMillis();
                m_timePassed = 0;
            }

        }

        lock.exit();
    }
}

// TODO make use of TrackingData
void TrackingStimulator::handleEvent (const EventChannel* eventInfo, const MidiMessage& event, int)
{
    if ((eventInfo->getName()).compare("Tracking data") != 0)
    {
        return;
    }

    BinaryEventPtr evtptr = BinaryEvent::deserializeFromMessage(event, eventInfo);

    if(event.getRawDataSize() != sizeof(TrackingData) + 18) { // TODO figure out why it is + 18
        cout << "Position tracker got wrong event size x,y,width,height was expected: " << event.getRawDataSize() << endl;
        return;
    }

//    const auto* rawData = (uint8*) evtptr->getBinaryDataPointer();
    auto nodeId = evtptr->getSourceID();
//    const int64 timestamp = *(rawData);
//    const float* message = (float*)(rawData+sizeof(int64));
    const auto *message = reinterpret_cast<const TrackingData *>(evtptr->getBinaryDataPointer());

    TrackingPosition position = message->position;

    if(!(position.x != position.x || position.y != position.y) && position.x != 0 && position.y != 0)
    {
        m_x = position.x;
        m_y = position.y;
    }
    if(!(position.width != position.width || position.height != position.height))
    {
        m_width = position.width;
        m_height = position.height;
        m_aspect_ratio = m_width / m_height;
    }
    m_positionIsUpdated = true;

    // Sync
    if (eventInfo->getChannelType() == EventChannel::TTL) // && eventInfo == eventChannelArray[triggerEvent])
    {
        TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, eventInfo);
        int eventId = ttl->getState() ? 1 : 0;

        std::cout << "In tracker stim" << std::endl;
        if (ttl->getChannel() ==  m_TTLSyncChan && eventId == 1)
        {
            {
                syncStimulation(m_StimSyncChan);
                CoreServices::sendStatusMessage("Sent trigger sync event!");
            }
        }
    }
}

int TrackingStimulator::isPositionWithinCircles(float x, float y)
{
    int whichCircle = -1;
    for (int i = 0; i < m_circles.size(); i++)
    {
        if (m_circles[i].isPositionIn(x,y))
            whichCircle = i;
    }
    return whichCircle;
}

bool TrackingStimulator::stimulate()
{
    if (isPositionWithinCircles(m_x, m_y) != -1)
        return true;
    else
        return false;
}

bool TrackingStimulator::positionDisplayedIsUpdated() const
{
    //return m_positionDisplayedIsUpdated;
    return m_positionIsUpdated;
}

void TrackingStimulator::clearPositionDisplayedUpdated()
{
    //m_positionDisplayedIsUpdated = false;
    m_positionIsUpdated = false;
}


bool TrackingStimulator::updatePulsePal()
{
    // check that Pulspal is connected and update param
    if (m_pulsePalVersion != 0)
    {
        int actual_chan = m_chan+1;
        float pulse_duration = 0;
        m_pulsePal.setBiphasic(actual_chan, m_isBiphasic[m_chan]);
        if (m_isBiphasic[m_chan])
        {
            //pulse_duration = float(m_phaseDuration[m_chan])/1000*2 + float(m_interPhaseInt[m_chan])/1000;
            m_pulsePal.setPhase1Duration(actual_chan, float(m_phaseDuration[m_chan])/1000);
            m_pulsePal.setPhase2Duration(actual_chan, float(m_phaseDuration[m_chan])/1000);
            m_pulsePal.setInterPhaseInterval(actual_chan, float(m_interPhaseInt[m_chan])/1000);
            if (m_negativeFirst[m_chan])
            {
                m_pulsePal.setPhase1Voltage(actual_chan, - m_voltage[m_chan]);
                m_pulsePal.setPhase2Voltage(actual_chan, m_voltage[m_chan]);
            }
            else
            {
                m_pulsePal.setPhase1Voltage(actual_chan, m_voltage[m_chan]);
                m_pulsePal.setPhase2Voltage(actual_chan, - m_voltage[m_chan]);
            }
        }
        else
        {
            pulse_duration = float(m_phaseDuration[m_chan])/1000;
            m_pulsePal.setPhase1Duration(actual_chan, float(m_phaseDuration[m_chan])/1000);
            m_pulsePal.setPhase2Duration(actual_chan, 0);
            m_pulsePal.setInterPhaseInterval(actual_chan, 0);
            if (m_negativeFirst[m_chan])
            {
                m_pulsePal.setPhase1Voltage(actual_chan, - m_voltage[m_chan]);
            }
            else
            {
                m_pulsePal.setPhase1Voltage(actual_chan, m_voltage[m_chan]);
            }

        }

        //float train_duration = float(m_interPulseInt[m_chan])/1000 * m_repetitions[m_chan] + float(m_phaseDuration[m_chan])/1000;
        m_pulsePal.setPulseTrainDuration(actual_chan, float(m_trainDuration[m_chan])/1000);

        m_pulsePal.setInterPulseInterval(actual_chan, float(m_interPulseInt[m_chan])/1000);

        return true;
    }
    else
        return false;
}

bool TrackingStimulator::testStimulation(){

    // check that Pulspal is connected and update param
    if (m_pulsePalVersion > 0)
    {
        m_pulsePal.triggerChannel(m_chan + 1);
        return true;
    }
    else
    {
        CoreServices::sendStatusMessage("PulsePal is not connected!");
        return false;
    }

} //test from Editor

bool TrackingStimulator::syncStimulation(int chan)
{
    if (m_pulsePalVersion > 0)
    {
        int actual_chan = chan+1;
        // Set channel to TTL pulse
        m_pulsePal.setBiphasic(actual_chan, 0);
        m_pulsePal.setPhase1Duration(actual_chan, float(5)/1000);
        m_pulsePal.setPhase1Voltage(actual_chan, 5.0);

        //float train_duration = float(m_interPulseInt[m_chan])/1000 * m_repetitions[m_chan] + float(m_phaseDuration[m_chan])/1000;
        m_pulsePal.setPulseTrainDuration(actual_chan, float(10)/1000);
        m_pulsePal.setInterPulseInterval(actual_chan, float(m_interPulseInt[m_chan])/1000);

        m_pulsePal.triggerChannel(actual_chan);

        //Reset channel info
        updatePulsePal();

        return true;
    }
    else
    {
        CoreServices::sendStatusMessage("PulsePal is not connected!");
        return false;
    }
}


void TrackingStimulator::startStimulation()
{
    m_isOn = true;

}

void TrackingStimulator::stopStimulation()
{
    m_isOn = false;
}

bool TrackingStimulator::isReady()
{
    return true;
}

bool TrackingStimulator::saveParametersXml()
{
    //Save
    XmlElement* state = new XmlElement("TrackingSTIMULATOR");

    // save circles
    XmlElement* circles = new XmlElement("CIRCLES");
    for (int i=0; i<m_circles.size(); i++)
    {
        XmlElement* circ = new XmlElement(String("Circles_")+=String(i));
        circ->setAttribute("id", i);
        circ->setAttribute("xpos", m_circles[i].getX());
        circ->setAttribute("ypos", m_circles[i].getY());
        circ->setAttribute("rad", m_circles[i].getRad());
        circ->setAttribute("on", m_circles[i].getOn());

        circles->addChildElement(circ);
    }
    // save stimulator conf
    XmlElement* channels = new XmlElement("CHANNELS");
    for (int i=0; i<4; i++)
    {
        XmlElement* chan = new XmlElement(String("Chan_")+=String(i+1));
        chan->setAttribute("id", i);
        chan->setAttribute("freq", m_stimFreq[i]);
        chan->setAttribute("sd", m_stimSD[i]);
        chan->setAttribute("uniform-gaussian", m_isUniform[i]);
        chan->setAttribute("biphasic", m_isBiphasic[i]);
        chan->setAttribute("negative-positive", m_negativeFirst[i]);
        chan->setAttribute("phase", m_phaseDuration[i]);
        chan->setAttribute("interphase", m_interPhaseInt[i]);
        chan->setAttribute("voltage", m_voltage[i]);
        chan->setAttribute("repetitions", m_repetitions[i]);
        chan->setAttribute("trainduration", m_trainDuration[i]);
        chan->setAttribute("interpulse", m_interPulseInt[i]);

        channels->addChildElement(chan);
    }


    state->addChildElement(circles);
    state->addChildElement(channels);

    if (! state->writeToFile(currentConfigFile, String::empty))
        return false;
    else
        return true;


}

bool TrackingStimulator::loadParametersXml(File fileToLoad)
{
    File currentFile = fileToLoad;

    XmlDocument doc(currentFile);
    XmlElement* xml = doc.getDocumentElement();

    if (xml == 0 || ! xml->hasTagName("TrackingSTIMULATOR"))
    {
        std::cout << "File not found." << std::endl;
        delete xml;
        return false;
    }
    else
    {

        forEachXmlChildElement(*xml, element)
        {
            if (element->hasTagName("CIRCLES"))
            {
                m_circles.clear();
                forEachXmlChildElement(*element, element2)
                {
                    int id = element2->getIntAttribute("id");
                    double cx = element2->getDoubleAttribute("xpos");
                    double cy = element2->getDoubleAttribute("ypos");
                    double crad = element2->getDoubleAttribute("rad");
                    bool on = element2->getIntAttribute("on");

                    Circle newCircle = Circle((float) cx, (float) cy, (float) crad, on);
                    m_circles.push_back(newCircle);

                }
            }
            if (element->hasTagName("CHANNELS"))
            {

                forEachXmlChildElement(*element, element2)
                {
                    int id = element2->getIntAttribute("id");
                    if (id<4) //pulse pal channels
                    {
                        double freq = element2->getDoubleAttribute("freq");
                        double sd = element2->getDoubleAttribute("sd");
                        int uni = element2->getIntAttribute("uniform-gaussian");
                        int biphasic = element2->getIntAttribute("biphasic");
                        int negfirst = element2->getIntAttribute("negative-positive");
                        double phase = element2->getDoubleAttribute("phase");
                        double interphase = element2->getDoubleAttribute("interphase");
                        double voltage = element2->getDoubleAttribute("voltage");
                        int rep = element2->getIntAttribute("repetitions");
                        double interpulse = element2->getDoubleAttribute("interpulse");
                        double trainduration = element2->getDoubleAttribute("trainduration");

                        m_stimFreq[id] = freq;
                        m_stimSD[id] = sd;
                        m_isUniform[id] = uni;
                        m_isBiphasic[id] = biphasic;
                        m_negativeFirst[id] = negfirst;
                        m_phaseDuration[id] = phase;
                        m_interPhaseInt[id] = interphase;
                        m_voltage[id] = voltage;
                        m_repetitions[id] = rep;
                        m_interPulseInt[id] = interpulse;
                        m_trainDuration[id] = trainduration;
                    }

                }
                break;
            }
        }
        return true;
    }
}

void TrackingStimulator::save()
{
    if (currentConfigFile.exists())
    {
        saveParametersXml();
    }
    else
    {
        FileChooser fc("Choose the file name...",
                       File::getCurrentWorkingDirectory(),
                       "*.xml");

        if (fc.browseForFileToSave(true))
        {
            currentConfigFile = fc.getResult();
            std::cout << currentConfigFile.getFileName() << std::endl;
            saveParametersXml();
        }
        else
        {
            CoreServices::sendStatusMessage("No file chosen!");
        }

    }
}

void TrackingStimulator::saveAs()
{
    FileChooser fc("Choose the file name...",
                   File::getCurrentWorkingDirectory(),
                   "*.xml");

    if (fc.browseForFileToSave(true))
    {
        currentConfigFile = fc.getResult();
        std::cout << currentConfigFile.getFileName() << std::endl;
        saveParametersXml();
    }
    else
    {
        CoreServices::sendStatusMessage("No file chosen!");
    }
}

void TrackingStimulator::load()
{
    FileChooser fc("Choose the file name...",
                   File::getCurrentWorkingDirectory(),
                   "*.xml");

    if (fc.browseForFileToOpen())
    {
        File fileToLoad = fc.getResult();
        std::cout << currentConfigFile.getFileName() << std::endl;
        loadParametersXml(fileToLoad);
    }
    else
    {
        CoreServices::sendStatusMessage("No file chosen!");
    }
}

void TrackingStimulator::saveCustomParametersToXml(XmlElement *parentElement)
{
    XmlElement* mainNode = parentElement->createNewChildElement("TrackingSTIMULATOR");
    // save circles
    XmlElement* circles = new XmlElement("CIRCLES");
    for (int i=0; i<m_circles.size(); i++)
    {
        XmlElement* circ = new XmlElement(String("Circles_")+=String(i));
        circ->setAttribute("id", i);
        circ->setAttribute("xpos", m_circles[i].getX());
        circ->setAttribute("ypos", m_circles[i].getY());
        circ->setAttribute("rad", m_circles[i].getRad());
        circ->setAttribute("on", m_circles[i].getOn());

        circles->addChildElement(circ);
    }
    // save stimulator conf
    XmlElement* channels = new XmlElement("CHANNELS");
    for (int i=0; i<4; i++)
    {
        XmlElement* chan = new XmlElement(String("Chan_")+=String(i+1));
        chan->setAttribute("id", i);
        chan->setAttribute("freq", m_stimFreq[i]);
        chan->setAttribute("sd", m_stimSD[i]);
        chan->setAttribute("uniform-gaussian", m_isUniform[i]);
        chan->setAttribute("biphasic", m_isBiphasic[i]);
        chan->setAttribute("negative-positive", m_negativeFirst[i]);
        chan->setAttribute("phase", m_phaseDuration[i]);
        chan->setAttribute("interphase", m_interPhaseInt[i]);
        chan->setAttribute("voltage", m_voltage[i]);
        chan->setAttribute("repetitions", m_repetitions[i]);
        chan->setAttribute("trainduration", m_trainDuration[i]);
        chan->setAttribute("interpulse", m_interPulseInt[i]);

        channels->addChildElement(chan);
    }

    mainNode->addChildElement(circles);
    mainNode->addChildElement(channels);

}

void TrackingStimulator::loadCustomParametersFromXml()
{
    if (parametersAsXml != nullptr)
    {
        forEachXmlChildElement(*parametersAsXml, mainNode)
        {
            if (mainNode->hasTagName("TrackingSTIMULATOR"))
            {
                forEachXmlChildElement(*mainNode, element)
                {
                    if (element->hasTagName("CIRCLES"))
                    {
                        m_circles.clear();
                        forEachXmlChildElement(*element, element2)
                        {
                            int id = element2->getIntAttribute("id");
                            double cx = element2->getDoubleAttribute("xpos");
                            double cy = element2->getDoubleAttribute("ypos");
                            double crad = element2->getDoubleAttribute("rad");
                            bool on = element2->getIntAttribute("on");

                            Circle newCircle = Circle((float) cx, (float) cy, (float) crad, on);
                            m_circles.push_back(newCircle);

                        }
                        //                        break;
                    }
                    if (element->hasTagName("CHANNELS"))
                    {

                        forEachXmlChildElement(*element, element2)
                        {
                            int id = element2->getIntAttribute("id");
                            if (id<4) //pulse pal channels
                            {
                                double freq = element2->getDoubleAttribute("freq");
                                double sd = element2->getDoubleAttribute("sd");
                                int uni = element2->getIntAttribute("uniform-gaussian");
                                int biphasic = element2->getIntAttribute("biphasic");
                                int negfirst = element2->getIntAttribute("negative-positive");
                                double phase = element2->getDoubleAttribute("phase");
                                double interphase = element2->getDoubleAttribute("interphase");
                                double voltage = element2->getDoubleAttribute("voltage");
                                int rep = element2->getIntAttribute("repetitions");
                                double interpulse = element2->getDoubleAttribute("interpulse");
                                double trainduration = element2->getDoubleAttribute("trainduration");

                                m_stimFreq[id] = freq;
                                m_stimSD[id] = sd;
                                m_isUniform[id] = uni;
                                m_isBiphasic[id] = biphasic;
                                m_negativeFirst[id] = negfirst;
                                m_phaseDuration[id] = phase;
                                m_interPhaseInt[id] = interphase;
                                m_voltage[id] = voltage;
                                m_repetitions[id] = rep;
                                m_interPulseInt[id] = interpulse;
                                m_trainDuration[id] = trainduration;
                            }

                        }
                        break;
                    }
                }

            }
        }
    }
}


// Circle methods

Circle::Circle()
    : m_cx(0),
      m_cy(0),
      m_rad(0)
{}

Circle::Circle(float x, float y, float rad, bool on)
{
    m_cx = x;
    m_cy = y;
    m_rad = rad;
    m_on = on;
}

float Circle::getX()
{
    return m_cx;
}
float Circle::getY()
{
    return m_cy;
}
float Circle::getRad()
{
    return m_rad;
}
bool Circle::getOn()
{
    return m_on;
}
void Circle::setX(float x)
{
    m_cx = x;
}
void Circle::setY(float y)
{
    m_cy = y;
}
void Circle::setRad(float rad)
{
    m_rad = rad;
}
void Circle::set(float x, float y, float rad, bool on)
{
    m_cx = x;
    m_cy = y;
    m_rad = rad;
    m_on = on;
}

bool Circle::on()
{
    m_on = true;
    return m_on;
}
bool Circle::off()
{
    m_on = false;
    return m_on;
}

bool Circle::isPositionIn(float x, float y)
{
    if (pow(x - m_cx,2) + pow(y - m_cy,2)
            <= m_rad*m_rad)
        return true;
    else
        return false;
}

float Circle::distanceFromCenter(float x, float y){
    return sqrt(pow(x - m_cx,2) + pow(y - m_cy,2));
}

