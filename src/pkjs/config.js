module.exports = [
  {
    type: 'heading',
    defaultValue: 'Mood Ring Face'
  },
  {
    type: 'text',
    defaultValue: 'Reads your heart rate and paints your mood from relaxed blue to anxious red.'
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Time'
      },
      {
        type: 'select',
        messageKey: 'TimeFormat',
        defaultValue: '0',
        label: 'Hour format',
        options: [
          { label: 'System default', value: '0' },
          { label: '12-hour', value: '1' },
          { label: '24-hour', value: '2' }
        ]
      },
      {
        type: 'toggle',
        messageKey: 'LeadingZero',
        label: 'Leading zero on hours',
        defaultValue: true
      },
      {
        type: 'toggle',
        messageKey: 'ShowDate',
        label: 'Show date',
        defaultValue: false
      }
    ]
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Character'
      },
      {
        type: 'select',
        messageKey: 'Character',
        defaultValue: '0',
        label: 'Face',
        options: [
          { label: 'Mia (girl)', value: '0' },
          { label: 'Kai (boy)', value: '1' },
          { label: 'Nova (glasses)', value: '2' },
          { label: 'Rex (beard)', value: '3' },
          { label: 'Cat', value: '4' },
          { label: 'Dog', value: '5' },
          { label: 'Fox', value: '6' },
          { label: 'Owl', value: '7' }
        ]
      }
    ]
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Mood'
      },
      {
        type: 'toggle',
        messageKey: 'ShowBpm',
        label: 'Show heart rate',
        defaultValue: true
      },
      {
        type: 'toggle',
        messageKey: 'ShowMoodLabel',
        label: 'Show mood label',
        defaultValue: true
      },
      {
        type: 'toggle',
        messageKey: 'DemoMode',
        label: 'Demo mode (cycle moods without HR)',
        defaultValue: false
      }
    ]
  },

  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'BPM thresholds'
      },
      {
        type: 'text',
        defaultValue: 'Relaxed ≤ first value · Calm ≤ second · Alert ≤ third · above that = Anxious'
      },
      {
        type: 'slider',
        messageKey: 'RelaxedMax',
        label: 'Relaxed max BPM',
        defaultValue: 65,
        min: 45,
        max: 90,
        step: 1
      },
      {
        type: 'slider',
        messageKey: 'CalmMax',
        label: 'Calm max BPM',
        defaultValue: 85,
        min: 60,
        max: 120,
        step: 1
      },
      {
        type: 'slider',
        messageKey: 'AlertMax',
        label: 'Alert max BPM',
        defaultValue: 105,
        min: 80,
        max: 150,
        step: 1
      }
    ]
  },

  {
    type: 'submit',
    defaultValue: 'Save'
  }
];
