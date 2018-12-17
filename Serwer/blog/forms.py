from django import forms

class ChooseChartForm(forms.Form):
    OPTIONS = (
        ("alcohol", "Alcohol in time"),
        ("steps", "Steps in time"),
        ("pulse", "Pulse in time"),
        ("activity", "Pulse-steps"),
        ("analysis2d", "Steps-alcohol"),
        ("analysis3d", "Pulse-alcohol-steps"),
    )
    charts = forms.MultipleChoiceField(widget=forms.CheckboxSelectMultiple,
                                          choices=OPTIONS)
