#include "midi_handles.h"

void HandleNoteOn(byte channel, byte note, byte velocity)
{





  int empty_arg = -1;
  for (byte i = 0; i < POLYPHONY; i++)  //take a non playing oscil
  {
    if (!envelope[i].playing())
    {
      empty_arg = i;
      break;
    }

  }

  if (empty_arg == -1)  //kill a oscil in release phase
  {
    byte min_rank = 255;
    for (byte i = 0; i < POLYPHONY; i++)
    {
      if (oscil_state[i] == 0 && oscil_rank[i] < min_rank)
      {
        empty_arg = i;
        min_rank = oscil_rank[i];
      }
    }

  }


  if (empty_arg != -1)   //an empty oscil has been found
  {

    carrier_freq[empty_arg] = Q16n16_mtof(Q8n0_to_Q16n16(note));
    aCarrier[empty_arg].setFreq_Q16n16(carrier_freq[empty_arg]);
    notes[empty_arg] = note;
    digitalWrite(LED, HIGH);
    envelope[empty_arg].noteOn();
    oscil_state[empty_arg] = 1;
    byte max_rank = 0;


    for (byte i = 0; i < POLYPHONY; i++)
    {
      if (oscil_rank[i] > max_rank) max_rank = oscil_rank[i];
    }
    oscil_rank[empty_arg] = max_rank + 1;
    set_freq(empty_arg);
  }


  else  // no empty oscil, kill one in sustain mode! (the oldest)
  {
    byte    min_rank = 255;
    byte min_rank_arg = 0;
    for (byte i = 0; i < POLYPHONY; i++)
    {
      if (oscil_rank[i] < min_rank)
      {
        min_rank = oscil_rank[i];
        min_rank_arg  = i;
      }
    }
    carrier_freq[min_rank_arg] = Q16n16_mtof(Q8n0_to_Q16n16(note));
    aCarrier[min_rank_arg].setFreq_Q16n16(carrier_freq[min_rank_arg]);
    notes[min_rank_arg] = note;
    digitalWrite(LED, HIGH);
    envelope[min_rank_arg].noteOn();
    oscil_state[min_rank_arg] = 1;
    set_freq(empty_arg);
    //for (byte i = 0; i < POLYPHONY; i++) oscil_rank[i] -= min_rank;


    byte max_rank = 0;
    for (byte i = 0; i < POLYPHONY; i++)
    {
      if (oscil_rank[i] > max_rank) max_rank = oscil_rank[i];
    }
    oscil_rank[min_rank_arg] = max_rank + 1;
    Serial.println(max_rank);

  }

 // shift all oscill
  byte min_rank = 255;
  for (byte i = 0; i < POLYPHONY; i++)
  {
    if (oscil_rank[i] < min_rank)
    {
      empty_arg = i;
      min_rank = oscil_rank[i];
    }
  }
  if (min_rank !=0) for (byte i = 0; i < POLYPHONY; i++) oscil_rank[i] -= min_rank;


}

void HandleNoteOff(byte channel, byte note, byte velocity)
{
  byte to_kill = 0;
  byte min_rank = 255;
  for (byte i = 0; i < POLYPHONY; i++)
  {
    if (note == notes[i] && oscil_state[i] == 1 && oscil_rank[i] < min_rank  )
    {
      min_rank = oscil_rank[i];
      to_kill = i;
    }
  }
  envelope[to_kill].noteOff();
  oscil_state[to_kill] = 0;
  digitalWrite(LED, LOW);
  //envelope[0].noteOff();
}

